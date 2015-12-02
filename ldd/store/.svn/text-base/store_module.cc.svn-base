

#include <set>
#include <glog/logging.h>
#include <boost/lexical_cast.hpp>
#include "config_manager.h"
#include "plant_pool.h"
#include "db/db.h"
#include "xleveldb/status.h"

#include "store_module.h"
#include "session.h"



LDD_SKEL_SIMPLE_MODULE(ldd::storage::StoreModule);


namespace ldd{
namespace storage{
	
StoreModule::StoreModule() : config_manager_(NULL),
	plant_pool_(NULL),
	db_impl_(NULL),
	listen_port_(0),
	read_thread_num_(0), 
	write_thread_num_(0){
}

StoreModule::~StoreModule(){
	delete config_manager_;
	delete plant_pool_;
	delete db_impl_;
}


bool StoreModule::InitModule(ModuleCtx* ctx){
    LOG(ERROR)<<"StoreModule::InitModule() restart!!!!!!!";
	listen_port_ = ctx->port();

    std::string		localhost = ctx->addr().ToString();

    ldd::net::Server::Options options;
    ldd::net::Client::Options client_opt;

    bool force_consistency = true;
    int recv_timeout = 0;
    int done_timeout = 0;
    int prepare_interval = 0;

	try{
		read_thread_num_= ctx->config().get<int>("leveldb.read_thread_num", 1);
		write_thread_num_= ctx->config().get<int>("leveldb.write_thread_num", 1);
        force_consistency  = ctx->config().get<bool>("leveldb.force_consistency", true);
        prepare_interval = ctx->config().get<int>("leveldb.prepare_interval", 5);

        std::string db_path_conf = "leveldb." + boost::lexical_cast<string>(listen_port_);
		db_path_=ctx->config().get(db_path_conf, "");

		zk_host_port_=ctx->config().get("zookeeper.host_port", "localhost:2181");
		zk_root_path_=ctx->config().get("zookeeper.root_path", "");


        options.pulse_interval   =  
                    ctx->config().get<int>("server.pulse_interval", 100);
        client_opt.pulse_interval   =  
                    ctx->config().get<int>("client.pulse_interval", 100);
        client_opt.connect_timeout  =  
                    ctx->config().get<int>("client.connect_timeout", 100);
        client_opt.resolve_timeout  =  
                    ctx->config().get<int>("client.resolve_timeout", 100);

        recv_timeout = ctx->config().get<int>("client.recv_timeout", 200);
        done_timeout = ctx->config().get<int>("client.done_timeout", 300);

		
		LOG(INFO)<<"localhost="<<localhost;
		LOG(INFO)<<" zk_host_port="<<zk_host_port_;
		LOG(INFO)<<" zk_root_path="<<zk_root_path_;
		LOG(INFO)<<" db_path="<<db_path_;
        LOG(INFO)<<" force_consistency="<<force_consistency;
        LOG(INFO)<<" db_path_conf"<<db_path_conf;
		LOG(INFO)<<" write_thread_num="<<write_thread_num_;
        LOG(INFO)<<" read_thread_num="<<read_thread_num_;
        LOG(INFO)<<" server pulse_interval="<<options.pulse_interval;
        LOG(INFO)<<" client.pulse_interval="<<client_opt.pulse_interval;
        LOG(INFO)<<" client.connect_timeout="<<client_opt.connect_timeout;
        LOG(INFO)<<" client.resolve_timeout="<<client_opt.resolve_timeout;
        LOG(INFO)<<" recv_timeout="<<recv_timeout;
        LOG(INFO)<<" done_timeout="<<done_timeout;
        LOG(INFO)<<" prepare_interval="<<prepare_interval;
		
	} catch (std::exception& e){
		return false;
	}


	config_manager_ = new ConfigManager(ctx->event_loop(), 
                    client_opt,
					localhost + ":" + boost::lexical_cast<string>(listen_port_),
                    force_consistency,
                    recv_timeout,
                    done_timeout,
                    prepare_interval);

    //options.notify_connected =
    //    boost::bind(&ExampleModule::OnConnected, this, _1);
    //options.notify_closed =
    //    boost::bind(&ExampleModule::OnClosed, this, _1);          // todo
    ldd::net::Server* server = new ldd::net::Server(options);
    if (!server->Start(ctx->listener())) {
        LOG(ERROR) << "Start server failed";
        return false;
    }

	server->RegisterIncomingMsg<ReadSession>(config_manager_);
	server->RegisterIncomingMsg<CheckSession>(config_manager_);
	server->RegisterIncomingMsg<MutateSession>(config_manager_);
	server->RegisterIncomingMsg<ListSession>(config_manager_);
	server->RegisterIncomingMsg<AtomicIncrSession>(config_manager_);
	server->RegisterIncomingMsg<AtomicAppendSession>(config_manager_);
	server->RegisterIncomingMsg<GetVersionSession>(config_manager_);
	server->RegisterIncomingMsg<MutationPrepareSession>(config_manager_);

	return RunStore();
}

bool StoreModule::RunStore(){
	
	
	option_.timeout = 400;
    option_.cb = boost::bind(&StoreModule::HandleZkError, this, _1);
	int ret = config_manager_->SetUpCourier(option_, zk_host_port_, zk_root_path_);
	if (ret != 0){
		LOG(ERROR)<<"StoreModule::RunStore() courier initialize error, ret="<<ret;
		return false;
	}

	const std::set<uint16_t>& bucket_set = config_manager_->GetBucketIdSet();
	db_impl_ = NULL;
	Status s = NewXLevelDB(db_path_, bucket_set, &db_impl_);

	if (s.ok() && db_impl_){
		config_manager_->SetDb(db_impl_);
		std::set<uint16_t>::const_iterator itr = bucket_set.begin();
		for (; itr!=bucket_set.end(); itr++){
			db_impl_->InsertBucket(*itr);
			LOG(INFO)<<"StoreModule::RunStore() insert bucket id:"<<*itr;
		}
	}else{
		LOG(ERROR)<<"StoreModule::RunStore() create db error: "<<s.ToString();
		return false;
	}

	plant_pool_	= new PlantPool(read_thread_num_, write_thread_num_);	// must >= 1		// need wait thread exit
	plant_pool_->SetUpPlantPool();

	return true;
}

void StoreModule::HandleZkError(int error_code){
    LOG(ERROR)<<"StoreModule::HandleZkError() error_code="<<error_code;
    exit(1);
}


}//namespace storage
}//namespace ldd


