#include <gflags/gflags.h>
#include <glog/logging.h>
#include "ldd/net/event_loop.h"
#include "ldd/net/client.h"

#include "../client.h"
#include "../provider.h"


using namespace ldd::client::raw;


DEFINE_string(zk_host_port, "0.0.0.0:2181", "zookeeper host_port");
DEFINE_int32(op, 1, "operation_type");

void PutCompletion(int64_t version){
	LOG(INFO)<<"callback() PutCompletion() version="<<version;
}

void DataCompletion2(int64_t version, const std::string& value, uint32_t ttl){
	LOG(INFO)<<"callback() DataCompletion() version="<<version<<" value="<<value<<" ttl="<<ttl;
}	

void CheckCompletion2(int64_t version, uint32_t ttl){
	LOG(INFO)<<"callback() CheckCompletion2() version="<<version<<" ttl="<<ttl;
}

void CasCompletion2(int64_t version, const std::string& value){
	LOG(INFO)<<"callback() CasCompletion2() version="<<version<<" value="<<value;
}


//./ut_client -zk_host_port=10.119.94.91:2181 -logtostderr=true -op=1
int main(int argc, char **argv){

	if (argc < 2){
		fprintf(stderr, "usage: 1 : Put\n");
		fprintf(stderr, "usage: 2 : Get\n");
		fprintf(stderr, "usage: 3 : Check\n");
		fprintf(stderr, "usage: 4 : List\n");
		fprintf(stderr, "usage: 5 : Append\n");
		fprintf(stderr, "usage: 6 : Incr\n");
		return 1 ;
	}
	
	google::InitGoogleLogging(argv[0]);
	google::ParseCommandLineFlags(&argc, &argv, true);

    ldd::net::EventLoop event_loop;
    ldd::net::Client::Options options;
    options.pulse_interval = 200;
    options.connect_timeout = 200;
    options.resolve_timeout = 200;
    //options.notify_connected = NotifyConnected;
    //options.notify_connecting = NotifyConnecting;
    //options.notify_closed = NotifyClosed;

    ldd::net::Client client(options);
	Provider* provider = NULL;
	
	Options option ;
	option.time_out = 10000;

	provider = new ProxyZkProvider(FLAGS_zk_host_port, "", 10000);
	

	Client* ct = NewClient(&client, &event_loop, provider, option);
	if (!ct){
		LOG(ERROR)<<"client create error";
		return 1;
	}
		
	LOG(INFO)<<"client init successfully";

	if (FLAGS_op == 1){
        LOG(INFO)<<"Put";
		ct->Put("ns1", "key1", "value1", PutCompletion);
	}else if(FLAGS_op == 2){
        LOG(INFO)<<"Get";
		ct->Get("ns1", "key1", DataCompletion2);
	}else if(FLAGS_op == 3){
        LOG(INFO)<<"Check";
		ct->Check("ns1", "key1", CheckCompletion2);
	}else if(FLAGS_op == 4){
        LOG(INFO)<<"List";
		ct->List("ns1", "", 100, 1,  DataCompletion2);
	}else if (FLAGS_op == 5){
        LOG(INFO)<<"Append";
		ct->Append("ns1", "key1", 1, "_append", 0, CasCompletion2 );
	}
	
	
    event_loop.Run();

	getchar();
	delete ct;	
	return 0;
}

