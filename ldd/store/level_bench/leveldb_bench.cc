
#include <stdint.h>
#include <glog/logging.h>
#include <gtest/gtest.h>	
#include <gflags/gflags.h>

#include <sys/time.h>
#include <time.h>
#include "histogram.h"
#include "random.h"
#include "ldd/util/slice.h"
#include "testutil.h"

#include "../db/db.h"
#include "../db/util.h"



using namespace ldd::storage;
DB*				g_db;


DEFINE_int32(thread, 1, "thread number");
DEFINE_int32(num, 10000, "record num");
DEFINE_string(module_name, "write", "module_name ");
DEFINE_string(path, "./data", "db path");
DEFINE_int32(var_len, 10, "approximate var length");



// Print histogram of operation timings
DEFINE_bool(histogram, true, " whether draw graph");
DEFINE_double(compression_ratio, 0.5, " compression_ratio");



DEFINE_int32(length, 100, "key length");

//#define KEY_LENGTH  100
/************************************************************************/
/* do the real bechmarch testing										*/
/************************************************************************/
class ThreadArg{
public:
	Stats* stat;
	typedef void (*MODULE_NAME)( ThreadArg* t);
	MODULE_NAME module_func_;
	//SynClient* client_;
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

void RunBenchmark(int thread_num, Slice module_name, ThreadArg::MODULE_NAME module_func){
    uint64_t start = NowMicros();
	ThreadArg* arg = new ThreadArg[thread_num];
	pthread_t* id = new pthread_t[thread_num];
	Stats* stats = new Stats[thread_num];

	for (int i=0; i<thread_num; i++){
        if (i<10){
            snprintf(arg[i].filename, 100, "GenerateFile_0%d", i);
        }else{
            snprintf(arg[i].filename, 100, "GenerateFile_%d", i);
        }
           
		arg[i].stat = &(stats[i]);
		arg[i].module_func_ = module_func;
		//arg[i].client_	= ct;
        arg[i].not_found = 0;
        arg[i].failed    = 0;
		pthread_create(&(id[i]), NULL, ThreadBody, &(arg[i]));
		fprintf(stdout, "pthread_create() module_name=%s\n", module_name.data());
	}

	void* status;
	for (int j=0; j<thread_num; j++){
		pthread_join(id[j], &status);
	}

	fprintf(stdout, "RunBenchmark() merge data\n");

    int read_not_found = 0;
    int total_failed = 0;
	for (int i = 1; i < thread_num; i++) {
		fprintf(stdout, "RunBenchmark() merge thread num=%d\n", i);
		stats[0].Merge(stats[1]);
        read_not_found +=arg[i].not_found;
        total_failed += arg[i].failed ;
	}
    read_not_found += arg[0].not_found;
    total_failed += arg[0].failed;

    double f_ratio = (double)total_failed / ((double)  FLAGS_thread * FLAGS_num);

    fprintf(stdout, "\n\nread_not_found=%d, failed=%d, f_ratio=%f\n\n", 
                           read_not_found, total_failed, f_ratio);
	stats[0].Report(module_name);

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
        //fprintf(stdout, "DoPut() key=%s, key=%d\n", key, (int)strlen(key));
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
        uint64_t seq=0; 
       // int code = arg->client_->Put(key, value);
        const Slice& rowkey(key);
        const Slice& db_value(value);
        Status s =  g_db->Put(0, 1, rowkey,Slice(),db_value,&seq,0);
        gettimeofday(&tv2, NULL);
        bytes += value_len + strlen(key);

        // fprintf(stdout, "DoPut() key=%s, value=%s\n", key, value.data());
        if (!s.ok()){
            fprintf(stdout, "DoPut() put error %s, %s\n", key, s.ToString().c_str());
            arg->failed++;
        }
        else{
            arg->stat->FinishedSingleOp(); 
        }
    }

    arg->stat->AddBytes(bytes);
    fclose(fp);
	fprintf(stdout, "write_random() byebye"); 
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
        //std::string value;
        //int code = arg->client_->Get(key, &value);

        const Slice rowkey(key);
        uint64_t    seq;
        uint64_t    ttl;

        std::string value;
        Status s =  g_db->Get(0, 1, rowkey, Slice(), &value, &seq, &ttl);
        bytes += value.size() + strlen(key);

        if (!s.ok()){
            if (s.IsNotFound()){
                arg->not_found++;
                arg->stat->FinishedSingleOp();
            }
            fprintf(stdout, "DoRead() Read error %s, %s\n", key, s.ToString().c_str());
            arg->failed++;
            
        }else{
            int e =  memcmp(key, value.c_str(), FLAGS_length);
            if (e != 0){
                fprintf(stdout, "DoRead() Read error key=%s, value=%s %s\n", 
                    key,  value.c_str(), s.ToString().c_str());
                exit(1);
            }
            arg->stat->FinishedSingleOp();
        }
    }

    arg->stat->AddBytes(bytes);
    fclose(fp);
}

char *rand_str(char *str,const int len)
{
    int i;
    for(i=0;i<len;++i)
        str[i]='A'+rand() % 26;
    str[i]='\0';
    return str;
}
void generate(){
    char file_name[100];
    for (int i=0; i<FLAGS_thread; i++){
        if (i<10){
            snprintf(file_name, 100, "GenerateFile_0%d", i);
        }else{
            snprintf(file_name, 100, "GenerateFile_%d", i);
        }
        FILE* fp = fopen(file_name, "w");

        srand(time(NULL) + i * 100);
        char* key = new char[FLAGS_length+1];
        for (int i=0; i<FLAGS_num; i++){
            rand_str(key, FLAGS_length);
            fprintf(fp, "%s\n", key);
        }
        fclose(fp);
    }
}

// ./leveldb_bench -num=20000 -thread=1  -module_name=write --log_dir="./"
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
		//module_func = del;
        fprintf(stdout, "del()------------- %s\n", start.data());
	}else if(FLAGS_module_name.compare("generate") == 0){
        generate();
        return 0;
      
    }else {
          CHECK(0);
    }

    std::set<uint16_t>bucket_set ;
    bucket_set.insert(0);
    bucket_set.insert(1);
    bucket_set.insert(2);
    g_db = NULL;

    std::string db_path;
    db_path = FLAGS_path;
    Status s = NewXLevelDB(db_path, bucket_set, &g_db);

    if (!s.ok())
    {
        fprintf(stderr, "NewXLevelDB()  error  %s\n", s.ToString().c_str());
        exit(1);
    }
	
	RunBenchmark(FLAGS_thread, module_name, module_func);

	LOG(INFO)<<"BYE BYE !!!!";
	google::ShutdownGoogleLogging();

    std::string end_ = TimeToString();
    uint64_t end_start = NowMicros() / 1000;
    double tps = (double)  FLAGS_thread * FLAGS_num * 1000 / (double)(end_start - now_start);

    fprintf(stdout, "request per second :          %f\n", tps);


    fprintf(stdout, "start time :              %s\n", start.data());
    fprintf(stdout, "end time :                   %s\n", end_.data());
	 
	// 增加一个write成功 即刻就读的case
	
	return 0;
}

