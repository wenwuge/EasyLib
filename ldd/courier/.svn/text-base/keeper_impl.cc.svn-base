
#include <glog/logging.h>
#include "keeper_impl.h"
#include "zkbroker_store.h"


namespace ldd{
namespace courier{

int Keeper::Init(const Options& option, const std::string& zk_host_port,
				  const std::string& home_host_port,
				  const std::string& root_path, Keeper** keeper_ptr){
	LOG(INFO)<<"Keeper::Init()";
	KeeperImpl *impl = new KeeperImpl;
	*keeper_ptr = impl;
	return impl->Init(option, zk_host_port, home_host_port, root_path, impl);
}

Keeper::Keeper(){

};

Keeper::~Keeper(){

};

KeeperImpl::KeeperImpl() : zk_broker_(NULL){

}
KeeperImpl::~KeeperImpl(){
	delete zk_broker_;
}

int KeeperImpl::Init(const Options& option, const std::string& zk_host_port, 
					 const std::string& home_host_port,
					 const std::string& root_path, KeeperImpl* impl){

	ZkBrokerStore* z = new ZkBrokerStore(home_host_port);
	impl->SetZooKeeperBroker(z);		 
	return z->RunFromConfig(option, zk_host_port, root_path);
}

int KeeperImpl::GetReplicaSize(int *size) { 
	return zk_broker_->GetReplicaSize(size);
}

int KeeperImpl::GetStoreEngine(std::string* engine_name) { 
	return zk_broker_->GetStoreEngine(engine_name);
}

int KeeperImpl::GetMaxBinLogFile(int *size) { 
	return zk_broker_->GetMaxBinLogFile(size);
}

int KeeperImpl::GetBucketSize(int *size) { 
	return zk_broker_->GetBucketSize(size);
}

int KeeperImpl::StartLeaderElection(LeaderShipChangedNotify cb){
	return zk_broker_->StartLeaderElection(cb);
}

std::set<uint16_t>&	 KeeperImpl::GetBucketSet(){
	return zk_broker_->GetBucketSet();
}

int KeeperImpl::AddNewRoles(const Roles& roles){
	return zk_broker_->AddNewRoles(roles);
}

int KeeperImpl::GetRoles(RolesChangedNotify cb, Roles* roles){
	return zk_broker_->GetRoles(cb, roles);
}

int KeeperImpl::GetAliveNodes(OnlineNodesChangedNotify cb, std::vector<std::string>* server_list){
	return zk_broker_->GetAliveNodes(cb, server_list);
}

void KeeperImpl::SetZooKeeperBroker(ZkBrokerStore* z){
	zk_broker_	= z;
}

}//namespace courier
}//namespace ldd











