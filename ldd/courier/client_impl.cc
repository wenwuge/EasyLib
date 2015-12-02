
#include <glog/logging.h>
#include "client_impl.h"
#include "zkbroker_client.h"



namespace ldd{
namespace courier{


int Client::Init(const Options& option, const std::string& host_port, const std::string& root_path, 
				 Client** broker_ptr){
	LOG(INFO)<<"ClientImpl::Init()";
	ClientImpl *impl = new ClientImpl;
	*broker_ptr = impl;
	return impl->Init(option, host_port, root_path, impl);
}

Client::Client(){

}

Client::~Client(){

}

ClientImpl::ClientImpl() : zk_client_broker_(NULL){

}

ClientImpl::~ClientImpl(){
	delete zk_client_broker_;
}

int ClientImpl::Init(const Options& option,const std::string& host_port, const std::string& root_path,
					 ClientImpl* impl){
	ZkBrokerClient* z = new ZkBrokerClient;
	impl->SetZooKeeperBroker(z);		 
	return z->RunFromConfig(option, host_port, root_path);
};


void ClientImpl::SetZooKeeperBroker(ZkBrokerClient* z){
	zk_client_broker_	= z;
}

int ClientImpl::GetProxyList(NodeListChangedNotify cb, 
	std::vector<std::string>* server_list)  {
	return zk_client_broker_->GetProxyList(cb, server_list);
}


}//namespace courier
}//namespace ldd










