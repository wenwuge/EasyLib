#include <stdint.h>

#include <glog/logging.h>
#include <gtest/gtest.h>	
#include <gflags/gflags.h>

#include <ldd/client/provider.h>
#include <ldd/syn_client/syn_client.h>

using namespace ldd::client::syn;

DEFINE_string(zk_host, "127.0.0.1:2181", "server host");
DEFINE_int32(thread_num, 1, "thread number");

DEFINE_int32(pulse, 1000, "pulse interval");
DEFINE_int32(conn, 1000, "connect interval");
DEFINE_int32(recv, 2000, "recv interval");
DEFINE_int32(done, 3000, "done interval");
DEFINE_string(ns, "CloudScan", "namespace");

DEFINE_int32(zk_list, 0, "whether get static host list from zk");
DEFINE_string(hosts, "10.120.88.70:50000,10.120.88.71:50000,10.120.88.72:50000,10.120.88.73:50000", "static hosts address");
DEFINE_int32(ttl, 0, "leveldb ttl");

ldd::client::syn::SynClient** g_client;

DEFINE_int32(key_len, 100, "key length");
DEFINE_int32(var_len, 100, "approximate var length");



void init(){

	Option option;
    option.retry_interval  = 1000;
    option.zk_host_port    = FLAGS_zk_host;
    option.name_space      = FLAGS_ns;
    option.pulse_interval  = FLAGS_pulse;
    option.recv_timeout    = FLAGS_recv;
    option.done_timeout    = FLAGS_done;
    option.ttl             = FLAGS_ttl * 1000;
    option.connect_timeout = FLAGS_conn;

    ldd::client::raw::Provider* provider;
    if (FLAGS_zk_list == false)
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
        exit(1);
    }
	for (int i=0; i<FLAGS_thread_num; i++){

        SynClient* ct = NULL;
        int ret = NewSynClient(&ct, provider, option);
        if (ret != 0){
            fprintf(stderr, "NewSynClient error=%d\n", ret);
            return;
        }
		g_client[i] = ct;
	}

}

char *rand_str(char *str,const int len)
{
    int i;
    for(i=0;i<len;++i)
        str[i]='A'+rand() % 60;
    str[i]='\0';
    return str;
}


void test(){
    char* buf1 = new char[FLAGS_key_len];
	char* key = rand_str(buf1, FLAGS_key_len);
	
	char* value = new char[FLAGS_var_len];
	char* value2 = rand_str(value, FLAGS_var_len);


	int code = g_client[0]->Put(key, std::string(value2, FLAGS_var_len));
    if (code != 0){
		fprintf(stdout, "DoPut() put error %s, %d\n", key, code);
    }else{
		fprintf(stdout, "DoPut() put ok, value=%s\n", value2);
	}

	if (code == 0 ){
		std::string r_value ;
		int ret = 0;
		ret = g_client[0]->Get(key, &r_value);
		if (ret == 0){
			if( strncmp(r_value.c_str(), value, r_value.size()) !=0 ){
				fprintf(stdout, "DoPut() get error %s, %d\n", r_value.c_str(), ret);
			}
			fprintf(stdout, "DoGet() get ok, value=%s\n", r_value.c_str());
		}else{
			fprintf(stdout, "DoGet() get error code=%d\n", ret);
		}
		//if (ret == 0){
	}
	
	{
		int ret = 0;
		ret = g_client[0]->Delete(key);
		if (ret == 0){
			fprintf(stdout, "DoGet() del ok, key=%s\n", key);
		}else{
			fprintf(stdout, "DoGet() del error code=%d\n", ret);
		}
	}
	
	if (code == 0 ){
		std::string r_value ;
		int ret = 0;
		ret = g_client[0]->Get(key, &r_value);
		if (ret == 0){
			if( strncmp(r_value.c_str(), value, r_value.size()) !=0 ){
				fprintf(stdout, "DoPut() get error %s, %d\n", r_value.c_str(), ret);
			}
			fprintf(stdout, "DoGet() get ok, value=%s\n", r_value.c_str());
		}else{
			fprintf(stdout, "DoGet() get error code=%d\n", ret);
		}
		//if (ret == 0){
	}
	
	delete buf1;
	delete value;
}

int main(int argc, char **argv){
	google::InitGoogleLogging(argv[0]);		// ./your_application --log_dir=.
	google::ParseCommandLineFlags(&argc, &argv, false);
	
	
	g_client = new SynClient* [FLAGS_thread_num];
	init();
	
	test();

	for (int i=0; i<FLAGS_thread_num; i++){
	 delete g_client[i];
	}
	
	delete g_client;

	return 0;
}

