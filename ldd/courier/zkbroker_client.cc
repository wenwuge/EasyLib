
#include <glog/logging.h>
#include "zkbroker_client.h"
#include "reactor.h"
#include "zkcontext.h"
#include "completion_callback_client.h"
#include "watcher_action_client.h"
#include "redo_policy.h"


namespace ldd{
namespace courier{

ZkBrokerClient::ZkBrokerClient(): 
				node_list_notify_(NULL){
}

ZkBrokerClient::~ZkBrokerClient(){
	DLOG(INFO)<<"ZkBrokerClient::~ZkBrokerClient()";
	ZooKeeperReactor* reactor_ = CurReactor();
	if (reactor_){
		reactor_->Close();
	}
}


int	ZkBrokerClient::LoadAppConfig(){ 
	RedoPolicy* rp = CurRedoPolicy()->Copy();  
	int ret = GetProxyList(rp);
	if (ret != 0){
		LOG(ERROR)<<"ZooKeeperBroker LoadAppConfig ERROR, ret:"<<ret;
		return ret;
	}
	// base class default implementation
	return 0 ;
}

int ZkBrokerClient::LoadBaseConfig(){
    LOG(INFO)<<"ZkBrokerClient::LoadBaseConfig() NULL impl return 0";
    return 0;
}
	
/************************************************************************/
/* virtual interface implementation									*/
/************************************************************************/


int ZkBrokerClient::GetProxyList(NodeListChangedNotify cb, 
							   std::vector<std::string>* server_list){
	if (CourierErrorCode() != 0 ){
	   LOG(ERROR)<<"current ZkBrokerClient::GetNodeList() ERROR="<<CourierErrorCode();
	   return CourierErrorCode();
	}

	if (!server_list){ 
		LOG(ERROR)<<"ERROR proxy: GerNodeList() invalid argument from client";
	   return kInvalidArgument;
	}
    server_list->clear();
	node_list_notify_ = cb;
    util::RWMutexLock rlock(GetRwLockPtr());
	std::vector<std::string>::const_iterator itr = node_collection_.begin();
	for (; itr != node_collection_.end(); itr++){
		server_list->push_back(*itr);
		DLOG(INFO)<<"ZkBrokerClient::GetNodeList(), host_port:"<<*itr;
	}
	return 0;
}


/************************************************************************/
/*these functions below need set callback and watcher					*/
/************************************************************************/
int ZkBrokerClient::GetProxyList(RedoPolicy* rp){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"current ZkBrokerClient::GetNodeList() cur state ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	CompletionCallBack * cb = new ClientNodeListCB(this);
	cb->SetRedoPolicy(rp);
	WatcherAction* action = new ClientNodeListWatcherAction(this);
	std::string	node_path		= GetRootPath() + PROXY_NODE_LIST_PATH; 
	bool ret = CurReactor()->GetChildrenList(node_path, cb, action, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"proxy:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		IncrItemNum();
		return 0;
	}
}
	
}//namespace courier
}//namespace ldd



	



