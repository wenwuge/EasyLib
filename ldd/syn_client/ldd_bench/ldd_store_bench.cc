#include <stdint.h>
#include <glog/logging.h>
#include <gtest/gtest.h>	
#include <gflags/gflags.h>

#include <sys/time.h>
#include <time.h>
#include "histogram.h"
#include "random.h"
#include "slice.h"
#include "testutil.h"
#include "syn_client.h"
#include "ldd/client/provider.h"

using namespace ldd::client::syn;


DEFINE_string(zk_host, "127.0.0.1:2181", "server host");
DEFINE_int32(thread, 1, "thread number");
DEFINE_int32(num, 10000, "record num");
DEFINE_string(module_name, "write", "module_name ");

DEFINE_int32(pulse, 1000, "pulse interval");
DEFINE_int32(conn, 1000, "connect interval");
DEFINE_int32(recv, 2000, "recv interval");
DEFINE_int32(done, 3000, "done interval");
DEFINE_string(ns, "txl_multi_data", "namespace");


DEFINE_int32(zk_list, 0, "get host list from zk");
DEFINE_string(hosts, "10.142.100.25:50000,10.142.100.26:50000,10.142.100.27:50000,10.142.100.28:50000,10.142.100.29:50000,10.142.100.30:50000,10.142.100.31:50000,10.142.100.32:50000,10.142.100.33:50000,10.142.100.34:50000", "static hosts address");


DEFINE_int32(ttl, 0, "leveldb ttl");

// Print histogram of operation timings
DEFINE_bool(histogram, true, " whether draw graph");
DEFINE_double(compression_ratio, 0.5, " compression_ratio");



DEFINE_int32(length, 100, "key length");
DEFINE_int32(var_len, 10, "approximate var length");


//#define KEY_LENGTH  100
/************************************************************************/
/* do the real bechmarch testing										*/
/************************************************************************/
class ThreadArg{
public:
    /*~ThreadArg(){
        delete client_;
    }*/
    ThreadArg(){
        client_ = NULL;
    }
	Stats* stat;
	typedef void (*MODULE_NAME)( ThreadArg* t);
	MODULE_NAME module_func_;
	SynClient* client_;
    char filename[100];
    int  not_found;
    int  failed;
};

static void *ThreadBody(void* data){
	ThreadArg* arg = (ThreadArg*) data;
	arg->stat->Start();
	arg->module_func_(arg);
	arg->stat->Stop();
	return NULL;
}

ThreadArg* g_arg = NULL;
pthread_t* g_id = NULL;
Stats* g_stats = NULL;


void RunBenchmark(int thread_num, Slice module_name, ThreadArg::MODULE_NAME module_func){
    uint64_t start = NowMicros();
	g_arg = new ThreadArg[thread_num];
	g_id = new pthread_t[thread_num];
	g_stats = new Stats[thread_num];

    Option option;
    option.retry_interval = 1000;
    option.zk_host_port   = FLAGS_zk_host;
    option.name_space     = FLAGS_ns;
    option.pulse_interval = FLAGS_pulse;
    option.recv_timeout   = FLAGS_recv;
    option.done_timeout   = FLAGS_done;
    option.ttl            = FLAGS_ttl * 1000;
    option.connect_timeout = FLAGS_conn;
    

    ldd::client::raw::Provider* provider;
    if (FLAGS_zk_list == 0 )
    {
        provider = new ldd::client::raw::ProxyStaticProvider(FLAGS_hosts.c_str());
    }else{
        provider = new ldd::client::raw::ProxyZkProvider(option.zk_host_port, 
            option.zk_base_path, option.zk_timeout);
    }
    bool b = provider->Init();
    if (!b)
    {
        fprintf(stderr, "provider init error");
        exit(5);
    }
    
    
    /*provider = new ldd::client::raw::ProxyZkProvider(option.zk_host_port, 
                                option.zk_base_path, option.zk_timeout);*/
    //char* hosts= "10.142.100.25:50000,10.142.100.26:50000";
    

	for (int i=0; i<thread_num; i++){
        if (i<10){
            snprintf(g_arg[i].filename, 100, "GenerateFile_0%d", i);
        }else{
            snprintf(g_arg[i].filename, 100, "GenerateFile_%d", i);
        }

        SynClient* ct = NULL;
        int ret = NewSynClient(&ct, provider, option);
        if (ret != 0){
            fprintf(stderr, "NewSynClient error=%d\n", ret);
            return;
        }
        
		g_arg[i].stat = &(g_stats[i]);
		g_arg[i].module_func_ = module_func;
		g_arg[i].client_	= ct;
        g_arg[i].not_found = 0;
        g_arg[i].failed    = 0;
		pthread_create(&(g_id[i]), NULL, ThreadBody, &(g_arg[i]));
		fprintf(stdout, "pthread_create() module_name=%s\n", module_name.data());
	}

	void* status;
	for (int j=0; j<thread_num; j++){
		pthread_join(g_id[j], &status);
	}

	fprintf(stdout, "RunBenchmark() merge data\n");

    int read_not_found = 0;
    int total_failed = 0;
	for (int i = 1; i < thread_num; i++) {
		fprintf(stdout, "RunBenchmark() merge thread num=%d\n", i);
		g_stats[0].Merge(g_stats[1]);
        read_not_found +=g_arg[i].not_found;
        total_failed += g_arg[i].failed ;
	}
    read_not_found += g_arg[0].not_found;
    total_failed += g_arg[0].failed;

    double f_ratio = (double)total_failed / ((double)  FLAGS_thread * FLAGS_num);
    fprintf(stdout, "\n\nread_not_found=%d, failed=%d, f_ratio=%f\n\n", 
                           read_not_found, total_failed, f_ratio);
	g_stats[0].Report(module_name);

     uint64_t end = NowMicros();
     if (end > start ){
        double read_not_found_qps = read_not_found * 1000000 /( (end - start));
        fprintf(stdout, "\n\read_not_found_qps=%f\n\n", read_not_found_qps);
     }  
}


void write(ThreadArg* arg){
    FILE* fp = fopen(arg->filename, "r");
    fprintf(stdout, "open file ok %s\n", arg->filename);
    if (!fp){
        fprintf(stdout, "open file erro \n");
        return ;
    }

    RandomGenerator gen;
    Random rand(100 + pthread_self());
    int64_t bytes = 0;
    int value_len = 0;

    char* key = new char[FLAGS_length+2];
    for (int i = 0; i < FLAGS_num; i ++) {
        fgets(key, FLAGS_length+2, fp);
        key[FLAGS_length] = '\0';
        //fprintf(stdout, "DoPut() key=%s\n", key);
        timeval tv1 = {};
        timeval tv2 = {};
        gettimeofday(&tv1, NULL);
        uint32_t rand1 = rand.Uniform(100);

        if (rand1 < 95) {
            // 7 - 13 kb
            value_len = rand.Uniform(7) + 7;
        } else {
            // 14 - 20 kb
            value_len = rand.Uniform(7) + 14;
        }
        value_len *= FLAGS_var_len;

        std::string value = key;
        value += gen.Generate(value_len).ToString();
        int code = arg->client_->Put(key, value);
        gettimeofday(&tv2, NULL);
        bytes += value_len + strlen(key);

        // fprintf(stdout, "DoPut() key=%s, value=%s\n", key, value.data());
        if (code != 0){
            /*
            int64_t start = tv1.tv_sec;
            start *= 1000;
            start += tv1.tv_usec / 1000;
            int64_t end = tv2.tv_sec;
            end *= 1000;
            end += tv2.tv_usec / 1000;
            */
            //exit(1);
            fprintf(stdout, "DoPut() put error %s, %d\n", key, code);
            arg->failed++;
        }else{
            //fprintf(stdout, "DoPut() Put ok\n");
            arg->stat->FinishedSingleOp(); 
        }
    }

    arg->stat->AddBytes(bytes);
    fclose(fp);
	fprintf(stdout, "write_random() byebye"); 
}

void read_after_write(ThreadArg* arg){
    FILE* fp = fopen(arg->filename, "r");
    fprintf(stdout, "open file ok %s\n", arg->filename);
    if (!fp){
        fprintf(stdout, "open file erro \n");
        return ;
    }

    RandomGenerator gen;
    Random rand(100 + pthread_self());
    int64_t bytes = 0;
    int value_len = 0;

    char* key = new char[FLAGS_length+2];
    for (int i = 0; i < FLAGS_num; i ++) {
        fgets(key, FLAGS_length+2, fp);
        key[FLAGS_length] = '\0';
        //fprintf(stdout, "DoPut() key=%s\n", key);
        timeval tv1 = {};
        timeval tv2 = {};
        gettimeofday(&tv1, NULL);
        uint32_t rand1 = rand.Uniform(100);

        if (rand1 < 95) {
            // 7 - 13 kb
            value_len = rand.Uniform(7) + 7;
        } else {
            // 14 - 20 kb
            value_len = rand.Uniform(7) + 14;
        }
        value_len *= FLAGS_var_len;

        std::string value = key;
        value += gen.Generate(value_len).ToString();
        int code = arg->client_->Put(key, value);
        gettimeofday(&tv2, NULL);
        bytes += value_len + strlen(key);

        if (code != 0){
            fprintf(stdout, "DoPut() put error %s, %d\n", key, code);
            arg->failed++;
        }else{
            arg->stat->FinishedSingleOp(); 
            /////////////////////// write success, begin read
            std::string value;
            int code = arg->client_->Get(key, &value);
            bytes += value.size() + strlen(key);
            if (code != 0){
                if (code == -531){
                    arg->not_found++;
                    arg->stat->FinishedSingleOp();
                }

                fprintf(stdout, "DoRead() Read error %s, %d\n", key, code);
                arg->failed++;
            }else{
                int e =  memcmp(key, value.c_str(), FLAGS_length);
                if (e != 0){
                    fprintf(stdout, "DoRead() Read error key=%s, value=%s %d\n", 
                        key,  value.c_str(), code);
                    exit(1);
                }
                arg->stat->FinishedSingleOp();
            }
        }
    }

    arg->stat->AddBytes(bytes);
    fclose(fp);
    fprintf(stdout, "read_after_write() byebye"); 
}


void read(ThreadArg* arg){
    RandomGenerator gen;
    Random rand(100 + pthread_self());

    FILE* fp = fopen(arg->filename, "r");
    //fprintf(stdout, "open file ok %s\n", arg->filename);

    if (!fp){
        fprintf(stdout, "open file erro \n");
        return ;
    }
    int64_t bytes = 0;

    char* key = new char[FLAGS_length+2];
    for (int i = 0; i < FLAGS_num; i ++) {
        fgets(key, FLAGS_length+2, fp);
        key[FLAGS_length] = 0;
        //fprintf(stdout, "DoRead() key=%s\n", key);
        std::string value;
        int code = arg->client_->Get(key, &value);
        bytes += value.size() + strlen(key);

        if (code != 0){
            if (code == -531){
                arg->not_found++;
                arg->stat->FinishedSingleOp();
            }

            fprintf(stdout, "DoRead() Read error %s, %d\n", key, code);
            arg->failed++;
            //exit(1);
        }else{
            int e =  memcmp(key, value.c_str(), FLAGS_length);
            if (e != 0){
                fprintf(stdout, "DoRead() Read error key=%s, value=%s %d\n", 
                    key,  value.c_str(), code);
                exit(1);
            }
            arg->stat->FinishedSingleOp();
        }
    }

    arg->stat->AddBytes(bytes);
    fclose(fp);
}

void del(ThreadArg* arg){
    RandomGenerator gen;
    Random rand(100 + pthread_self());
    FILE* fp = fopen(arg->filename, "r");

    char* key = new char[FLAGS_length+2];

    for (int i = 0; i < FLAGS_num; i ++) {
        //const int k = sequence ? i: (rand.Next() % FLAGS_num);
        fgets(key, FLAGS_length+2, fp);
        key[FLAGS_length] = 0;
        //fprintf(stdout, "DoDelete() key=%s\n", key);
        int code = arg->client_->Delete(key);
        if (code != 0){
            fprintf(stdout, "DoDelete() del error， code=%d\n", code);
            //exit(1);
        }else{
            //fprintf(stdout, "DoDelete() del ok\n");
            arg->stat->FinishedSingleOp();
        }
    }

    fclose(fp);
}

char *rand_str(char *str,const int len)
{
    int i;
    for(i=0;i<len;++i)
        str[i]='A'+rand() % 60;
    str[i]='\0';
    return str;
}

void multi(ThreadArg* arg){
	RandomGenerator gen;
	Random rand(100 + pthread_self());
	FILE* fp = fopen(arg->filename, "r");

	char* key = new char[FLAGS_length+2];

	std::map<std::string, std::pair<std::string, uint64_t> >kvs;
	for (int i = 0; i < FLAGS_num/3; i ++) {
		kvs.clear();
		{
			fgets(key, FLAGS_length+2, fp);
			key[FLAGS_length] = 0;
			std::pair<std::string, uint64_t> p;
			p.first = key;
			p.second = 0;
			kvs.insert(make_pair(std::string(key), p));
		}

		{
			fgets(key, FLAGS_length+2, fp);
			key[FLAGS_length] = 0;
			std::pair<std::string, uint64_t> p;
			p.first = key;
			p.second = 0;
			kvs.insert(make_pair(std::string(key), p));
		}

		{
			fgets(key, FLAGS_length+2, fp);
			key[FLAGS_length] = 0;
			std::pair<std::string, uint64_t> p;
			p.first = key;
			p.second = 0;
			kvs.insert(make_pair(std::string(key), p));
		}

		
		
		//fprintf(stdout, "DoDelete() key=%s\n", key);
		int code = arg->client_->Put(kvs);
		if (code != 0){
			fprintf(stdout, "multi() del error， code=%d\n", code);
			//exit(1);
		}else{
			//fprintf(stdout, "DoDelete() del ok\n");
			arg->stat->FinishedSingleOp();
		}
	}

	std::map<std::string, std::pair<std::string, uint64_t> >::iterator itr = kvs.begin();
	for (; itr!=kvs.end(); itr++){
		fprintf(stderr, "multi() put: key=%s\n", itr->first.c_str());
		fprintf(stderr, "multi() put: value=%s\n", itr->second.first.c_str());
		fprintf(stderr, "multi() put: code=%lu\n", itr->second.second);
	}
	
	fprintf(stderr, "multi()\n\n");
	fseek(fp,0 ,SEEK_SET);


	std::map<std::string, std::pair<uint64_t, std::string> >kvs_read;

	for (int i = 0; i < FLAGS_num/3; i ++) {
		kvs_read.clear();
		{
			fgets(key, FLAGS_length+2, fp);
			key[FLAGS_length] = 0;
			std::pair<uint64_t, std::string> p;
			p.first = 0;
			p.second = "";
			kvs_read.insert(make_pair(std::string(key), p));
		}

		{
			fgets(key, FLAGS_length+2, fp);
			key[FLAGS_length] = 0;
			std::pair<uint64_t, std::string> p;
			p.first = 0;
			p.second = "";
			kvs_read.insert(make_pair(std::string(key), p));
		}

		{
			fgets(key, FLAGS_length+2, fp);
			key[FLAGS_length] = 0;
			std::pair<uint64_t, std::string> p;
			p.first = 0;
			p.second = "";
			kvs_read.insert(make_pair(std::string(key), p));
		}



		//fprintf(stdout, "DoDelete() key=%s\n", key);
		int code = arg->client_->Get(kvs_read);
		if (code != 0){
			fprintf(stdout, "DoDelete() del error， code=%d\n", code);
			//exit(1);
		}else{
			//fprintf(stdout, "DoDelete() del ok\n");
			arg->stat->FinishedSingleOp();
		}
	}

	{
		std::map<std::string, std::pair<uint64_t, std::string> >::iterator itr = kvs_read.begin();
		for (; itr!=kvs_read.end(); itr++)
		{
			fprintf(stderr, "multi() read: key=%s\n", itr->first.c_str());
			fprintf(stderr, "multi() read: code=%lu\n", itr->second.first);
			fprintf(stderr, "multi() read: value=%s\n", itr->second.second.c_str());
		}
	}

	fclose(fp);
}

void generate(){
    char file_name[100];
    timeval tv;
    
    for (int i=0; i<FLAGS_thread; i++){
        if (i<10){
            snprintf(file_name, 100, "GenerateFile_0%d", i);
        }else{
            snprintf(file_name, 100, "GenerateFile_%d", i);
        }
        FILE* fp = fopen(file_name, "w");

        gettimeofday(&tv, NULL);
        srand(tv.tv_usec + i * 100);
        char* key = new char[FLAGS_length+1];
        for (int i=0; i<FLAGS_num; i++){
            rand_str(key, FLAGS_length);
            fprintf(fp, "%s\n", key);
        }
        fclose(fp);
    }
}

// ./ldd_bench  --zk_host="10.119.94.91:2181"   --num=1 -thread=1 --module_name=write 
int main(int argc, char **argv){

	google::InitGoogleLogging(argv[0]);		// ./your_application --log_dir=.
	google::ParseCommandLineFlags(&argc, &argv, false);

    //FLAGS_logtostderr=true;
    std::string start = TimeToString();

    uint64_t now_start = NowMicros() / 1000;
	ThreadArg::MODULE_NAME module_func;
	Slice module_name;
	
	if (FLAGS_module_name.compare("write") == 0 ){
		module_name = FLAGS_module_name;
		module_func = write;
       fprintf(stdout, "write()------------- %s\n", start.data());
	}else if (FLAGS_module_name.compare("read") == 0 ){
		module_name = FLAGS_module_name;
		module_func = read;
         fprintf(stdout, "read()------------- %s\n", start.data());
	}else if (FLAGS_module_name.compare("del") == 0 ){
		module_name = FLAGS_module_name;
		module_func = del;
        fprintf(stdout, "del()------------- %s\n", start.data());
    }else if (FLAGS_module_name.compare("read_now") == 0 ){
        module_name = FLAGS_module_name;
        module_func = read_after_write;
        fprintf(stdout, "del()------------- %s\n", start.data());
	}else if (FLAGS_module_name.compare("multi") == 0 ){
		module_name = FLAGS_module_name;
		module_func = multi;
		fprintf(stdout, "multi()------------- %s\n", start.data());
	}else if(FLAGS_module_name.compare("generate") == 0){
        generate();
        return 0;
    }else {
          CHECK(0);
    }
	
	RunBenchmark(FLAGS_thread, module_name, module_func);

	LOG(INFO)<<"BYE BYE !!!!";
	


     std::string end_ = TimeToString();
     uint64_t end_start = NowMicros() / 1000;
     double tps = (double)  FLAGS_thread * FLAGS_num * 1000 / (double)(end_start - now_start);
    
     fprintf(stdout, "request per second :          %f\n", tps);
     
    
     fprintf(stdout, "start time :              %s\n", start.data());
     fprintf(stdout, "end time :                %s\n", end_.data()); 
	// 增加一个write成功 即刻就读的case
	
     for (int i=0; i<FLAGS_thread; i++){
         delete g_arg[i].client_;
     }

    google::ShutdownGoogleLogging();
	return 0;
}

