

#include <glog/logging.h>
#include <boost/lexical_cast.hpp>
#include <ldd/net/server.h>

#include "proxy.h"
#include "proxy_module.h"

#include "c2p/atomic_append.h"
#include "c2p/atomic_incr.h"
#include "c2p/check.h"
#include "c2p/get.h"
#include "c2p/list.h"
#include "c2p/mutate.h"



LDD_SKEL_SIMPLE_MODULE(ProxyModule);


	
ProxyModule::ProxyModule(){
}

ProxyModule::~ProxyModule(){
    LOG(ERROR)<<" ProxyModule::~ProxyModule()";
    delete proxy_;
}

bool ProxyModule::InitModule(ModuleCtx* ctx){
	listen_port_ = ctx->port();

    std::string		localhost = ctx->addr().ToString();

    ldd::net::Client::Options client_opt;

    ldd::net::Server::Options options;

    int recv_timeout = 0;
    int done_timeout = 0;

    int role_type = 0;
    bool proxy_stat = false;

	try{
        role_type =  ctx->config().get<int>("leveldb.role_type", 0);
        proxy_stat =  ctx->config().get<bool>("leveldb.proxy_stat", false);
        

        zookeeper_host_ = ctx->config().get("zookeeper.host_port", "localhost:2181");
        zookeeper_rootpath_ = ctx->config().get("zookeeper.root_path", "");
        zookeeper_timeout_  = ctx->config().get<int>("zookeeper.timeout", 400);


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
        

		
		LOG(INFO)<<"thread_number="<<role_type;
        LOG(INFO)<<" localhost="<<localhost;
		LOG(INFO)<<" zookeeper_host_="<<zookeeper_host_;
		LOG(INFO)<<" zookeeper_rootpath_="<<zookeeper_rootpath_;
		LOG(INFO)<<" zookeeper_timeout_="<<zookeeper_timeout_;
        LOG(INFO)<<" server pulse_interval="<<options.pulse_interval;
        LOG(INFO)<<" client.pulse_interval="<<client_opt.pulse_interval;
        LOG(INFO)<<" client.connect_timeout="<<client_opt.connect_timeout;
        LOG(INFO)<<" client.resolve_timeout="<<client_opt.resolve_timeout;
        LOG(INFO)<<" recv_timeout="<<recv_timeout;
        LOG(INFO)<<" done_timeout="<<done_timeout;
		
	} catch (std::exception& e){
		return false;
	}

    proxy_ = new Proxy(ctx->event_loop(), client_opt, 
        localhost + ":" + boost::lexical_cast<std::string>(listen_port_),
                        zookeeper_host_,
                        zookeeper_rootpath_,
                        zookeeper_timeout_,
                        recv_timeout, 
                        done_timeout,
                        role_type,
                        proxy_stat);

    if (!proxy_->Init()) {
        LOG(ERROR) << "proxy init failed";
        return false;
    }

    //options.notify_connected =
    //    boost::bind(&ExampleModule::OnConnected, this, _1);
    //options.notify_closed =
    //    boost::bind(&ExampleModule::OnClosed, this, _1);

    ldd::net::Server* server = new ldd::net::Server(options);
    if (!server->Start(ctx->listener())) {
        LOG(ERROR) << "Start server failed";
        return false;
    }

	server->RegisterIncomingMsg<C2PGet>(proxy_);
	server->RegisterIncomingMsg<C2PList>(proxy_);
	server->RegisterIncomingMsg<C2PCheck>(proxy_);
	server->RegisterIncomingMsg<C2PMutate>(proxy_);
	server->RegisterIncomingMsg<C2PAtomicIncr>(proxy_);
	server->RegisterIncomingMsg<C2PAtomicAppend>(proxy_);

	return true;
}

