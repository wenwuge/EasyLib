
#include <glog/logging.h>
#include "configure_impl.h"
#include "zkbroker_proxy.h"



namespace ldd{
namespace courier{


int Configure::Init(const Options& option, const std::string& home_host_port,
                    const std::string& host_port, const std::string& root_path, 
				    Configure** broker_ptr){
	LOG(INFO)<<"Configure::Init()";
	ConfigureImpl *impl = new ConfigureImpl;
	*broker_ptr = impl;
	return impl->Init(option,home_host_port, host_port, root_path, impl);
}

Configure::Configure(){

}

Configure::~Configure(){

}

ConfigureImpl::ConfigureImpl() : zk_proxy_broker_(NULL){

}

ConfigureImpl::~ConfigureImpl(){
	delete zk_proxy_broker_;
}

int ConfigureImpl::Init(const Options& option,
                        const std::string& home_host_port,
                        const std::string& host_port, 
                        const std::string& root_path,
					    ConfigureImpl* impl){
	ZkBrokerProxy* z = new ZkBrokerProxy(home_host_port);
	impl->SetZooKeeperBroker(z);		 
	return z->RunFromConfig(option, host_port, root_path);
};

int ConfigureImpl::GetReplicaSize(int *size) { 
	return zk_proxy_broker_->GetReplicaSize(size);
}

int ConfigureImpl::GetStoreEngine(std::string* engine_name) { 
	return zk_proxy_broker_->GetStoreEngine(engine_name);
}

int ConfigureImpl::GetMaxBinLogFile(int *size) { 
	return zk_proxy_broker_->GetMaxBinLogFile(size);
}

int ConfigureImpl::GetBucketSize(int *size) { 
	return zk_proxy_broker_->GetBucketSize(size);
}

int ConfigureImpl::GetNameSpaceID(const std::string& ns, int* id){
	return zk_proxy_broker_->GetNameSpaceID(ns, id);
}

int ConfigureImpl::FindFarmIdInBucketList(const int bucket_id, int* farm_id){ 
	return zk_proxy_broker_->FindFarmIdInBucketList(bucket_id, farm_id);
}

int ConfigureImpl::GetRoles(const int farm_id, Roles** roles){ 
	return zk_proxy_broker_->GetRoles(farm_id, roles);
}

int ConfigureImpl::GetNodeList(NodeListChangedNotify cb, 
	std::vector<std::string>* server_list)  {
	return zk_proxy_broker_->GetNodeList(cb, server_list);
}

int ConfigureImpl::GetFarmList(std::vector<int>** farm_list) {
	return zk_proxy_broker_->GetFarmList(farm_list);
}

void ConfigureImpl::SetZooKeeperBroker(ZkBrokerProxy* z){
	zk_proxy_broker_	= z;
}


}//namespace courier
}//namespace ldd










