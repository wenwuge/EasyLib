
#include <glog/logging.h>
#include "zkbroker_proxy.h"
#include "reactor.h"
#include "zkcontext.h"
#include "completion_callback_proxy.h"
#include "watcher_action_proxy.h"
#include "watcher_action_impl.h"
#include "redo_policy.h"

namespace ldd{
namespace courier{

ZkBrokerProxy::ZkBrokerProxy(const std::string& home_host_port):
				node_list_notify_(NULL),
                home_host_port_(home_host_port){
}

ZkBrokerProxy::~ZkBrokerProxy(){
	DLOG(INFO)<<"ZkBrokerProxy::~ZkBrokerProxy()";
	ZooKeeperReactor* reactor_ = CurReactor();
	if (reactor_){
		reactor_->Close();
	}
}
	
int ZkBrokerProxy::LoadAppConfig(){ 
	RedoPolicy* rp = CurRedoPolicy()->Copy();  
	int ret = GetNodeList(rp);
	if (ret != 0){
		LOG(ERROR)<<"ZooKeeperBroker GetMaxBinLogFile ERROR, ret:"<<ret;
		return ret;
	}
	rp = CurRedoPolicy()->Copy();
	ret = GetNsIdList(rp);
	if (ret != 0){
		LOG(ERROR)<<"ZooKeeperBroker GetNameSpaceID ERROR, ret:"<<ret;
		return ret;
	}

	rp = CurRedoPolicy()->Copy();
	ret = GetFarmList(rp);		    
	if (ret != 0){
		LOG(ERROR)<<"ZooKeeperBroker GetMaxBinLogFile ERROR, ret:"<<ret;
		return ret;
	}

    rp = CurRedoPolicy()->Copy();
    ret = CreateProxyNode(rp);		    
    if (ret != 0){
        LOG(ERROR)<<"ZooKeeperBroker GetMaxBinLogFile ERROR, ret:"<<ret;
        return ret;
    }

	return 0;
}

/************************************************************************/
/* virtual interface implementation									*/
/************************************************************************/
int ZkBrokerProxy::GetNameSpaceID(const std::string& ns, int* id) {
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"current ZkBrokerProxy::GetNameSpaceID() ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	if (!id){
		LOG(ERROR)<<"proxy: id invalid argument from client";
		return kInvalidArgument;
	}
	*id = -1;
    util::RWMutexLock rlock(GetRwLockPtr());
	map<string, int>::const_iterator itr = ns_collection_.begin();
	itr = ns_collection_.find(ns);
	if (itr == ns_collection_.end()){
		DLOG(INFO)<< "not find ns= "<<ns;
		return kNotFound;  
	}
	*id = itr->second;
	DLOG(INFO)<< "find ns= "<<ns<<" ns id= "<<*id;
	return kCourierOk;
}

//int ZkBrokerProxy::FindFarmIdInBucketList(const int bucket_id, 
//											  int* farm_id){
//	if (CourierErrorCode() != 0 ){
//		LOG(ERROR)<<"current ZkBrokerProxy::FindFarmIdInBucketList() ERROR="<<CourierErrorCode();
//		return CourierErrorCode();
//	}
//	if (!farm_id){
//		LOG(ERROR)<<"proxy: farmid invalid argument from client";
//		return kInvalidArgument;
//	}
//	*farm_id = -1;
//	char buf[20];
//	int len = snprintf(buf, sizeof(buf), "%d", bucket_id);
//	std::string sBucket_id(buf, len);
//	DLOG(INFO)<<"ZkBrokerProxy::FindFarmIdInBucketList() sBucket_id="
//                <<sBucket_id<<" bucket_id="<<bucket_id;
//
//    util::RWMutexLock rlock(GetRwLockPtr());
//	map<string, string>::const_iterator itr = bucket_collection_.begin();
//	itr = bucket_collection_.find(sBucket_id);
//	if (itr == bucket_collection_.end()){
//		DLOG(INFO)<< "ZkBrokerProxy::FindFarmIdInBucketList() not find bucketid= "
//            <<bucket_id<<" sBucket_id="<<sBucket_id;
//		return kNotFound;  
//	}
//	const std::string& str  = itr->second;
//	if (str[0]>= '0' && str[0]<= '9'){
//		*farm_id = atoi(str.data());
//		DLOG(INFO)<< "ZkBrokerProxy::FindFarmIdInBucketList() find farmid="
//            <<*farm_id<< " bucketid="<<bucket_id<<" sBucket_id="<<sBucket_id;
//		return kCourierOk;
//	}else{
//		DLOG(INFO)<< "ZkBrokerProxy::FindFarmIdInBucketList() not digit error farm= "<<str;
//		return kNotFound;
//	}
//}

int ZkBrokerProxy::FindFarmIdInBucketList(const int bucket_id, 
                                          int* farm_id){
    if (CourierErrorCode() != 0 ){
      LOG(ERROR)<<"current ZkBrokerProxy::FindFarmIdInBucketList() ERROR="<<CourierErrorCode();
      return CourierErrorCode();
    }
    if (!farm_id){
      LOG(ERROR)<<"proxy: farmid invalid argument from client";
      return kInvalidArgument;
    }
    *farm_id = -1;

    util::RWMutexLock rlock(GetRwLockPtr());
    map<int, int>::const_iterator itr = bucket_farmid_collection_.begin();
    itr = bucket_farmid_collection_.find(bucket_id);
    if (itr == bucket_farmid_collection_.end()){
      DLOG(INFO)<< "ZkBrokerProxy::FindFarmIdInBucketList() not find bucketid= "
          <<bucket_id<<" sBucket_id="<<bucket_id;
      return kNotFound;  
    }
    *farm_id= itr->second;
    DLOG(INFO)<< "ZkBrokerProxy::FindFarmIdInBucketList() find farmid="
      <<*farm_id<< " bucketid="<<bucket_id;
    return kCourierOk;
}

int ZkBrokerProxy::GetFarmList(std::vector<int>** farm_list) {
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ERROR current ZkBrokerProxy::GetRoles() ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}

	if (!farm_list){
		LOG(ERROR)<<"ERROR proxy: GetFarmList() invalid argument from client";
		return kInvalidArgument;
	}
	 util::RWMutexLock rlock(GetRwLockPtr());
	 *farm_list = &farm_collection_;
	 return kCourierOk;
}

int ZkBrokerProxy::GetRoles(const int farm_id, Roles** roles){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ERROR current ZkBrokerProxy::GetRoles() ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}

	if (!roles){
		LOG(ERROR)<<"ERROR proxy: GerRoles() invalid argument from client";
		return kInvalidArgument;
	}
    util::RWMutexLock rlock(GetRwLockPtr());
	map<int, Roles>::iterator itr ;
	itr = farm_roles_map_.find(farm_id);
	if (itr != farm_roles_map_.end()){
		*roles = &(itr->second); // todo   improve performance
		DLOG(INFO)<<"ZkBrokerProxy::GetRoles()  master:"<<(*roles)->master
                        <<" slave.size="<<(*roles)->slaves.size();
		return 0;
	}else{
		DLOG(INFO)<<"ZkBrokerProxy::GetRoles  no data: farm_id: "<<farm_id;
		return kNotFound;
	}
}

int ZkBrokerProxy::GetNodeList(NodeListChangedNotify cb, 
							   std::vector<std::string>* server_list){
	if (CourierErrorCode() != 0 ){
	   LOG(ERROR)<<"current ZkBrokerProxy::GetNodeList() ERROR="<<CourierErrorCode();
	   return CourierErrorCode();
	}

	if (!server_list){ 
		LOG(ERROR)<<"ERROR proxy: GerNodeList() invalid argument from client";
	   return kInvalidArgument;
	}
	node_list_notify_ = cb;
    util::RWMutexLock rlock(GetRwLockPtr());
	std::vector<std::string>::const_iterator itr = node_collection_.begin();
	for (; itr != node_collection_.end(); itr++){
		server_list->push_back(*itr);
		DLOG(INFO)<<"ZkBrokerProxy::GetNodeList(), host_port:"<<*itr;
	}
	return 0;
}




/************************************************************************/
/*these functions below need set callback and watcher					*/
/************************************************************************/
int ZkBrokerProxy::GetNodeList(RedoPolicy* rp){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"current ZkBrokerProxy::GetNodeList() cur state ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	CompletionCallBack * cb = new NodeListCB(this);
	cb->SetRedoPolicy(rp);
	WatcherAction* action = new PnodeListWatcherAction(this);
	std::string	node_path		= GetRootPath() + NODE_LIST_PATH; 
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

int ZkBrokerProxy::CreateProxyNode(RedoPolicy* rp){
    if (CourierErrorCode() != 0 ){
        LOG(ERROR)<<"current ZkBrokerProxy::GetNodeList() cur state ERROR="<<CourierErrorCode();
        return CourierErrorCode();
    }
    CompletionCallBack * cb = new CreateProxyNodeCB(this);
    cb->SetRedoPolicy(rp);
    std::string	node_path		= GetRootPath() + PROXY_NODE_LIST_PATH + "/" + home_host_port_; 
    bool ret = CurReactor()->CreateNode(node_path, home_host_port_, ZOO_EPHEMERAL, 
                                    cb, NULL, 
                                    rp->GetRedoTimeInterval());
    LOG(INFO)<<" ZkBrokerProxy::CreateProxyNode() path="<<node_path
                <<" value="<<home_host_port_;
    if (ret == false){
        SetCourierErrorCode(kZooKeeperNotWork);
        LOG(ERROR)<<"proxy:: SetZkErrorCode ERROR, kZkNotWork";
        return kZooKeeperNotWork; 
    }else{
        IncrItemNum();
        return 0;
    }
}

int ZkBrokerProxy::GetFarmList( RedoPolicy* rp ){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ZkBrokerProxy::GetFarmList() cur state  ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	CompletionCallBack * cb		= new FarmListCB(this);
	cb->SetRedoPolicy(rp);
	WatcherAction* action		= new FarmListWatcherAction(this);
	std::string	node_path		= GetRootPath() + FARM_LIST_PATH; 
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

int ZkBrokerProxy::GetNsIdList(RedoPolicy* rp){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ZkBrokerProxy::GetNameSpaceID() cur state  ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	CompletionCallBack * cb		= new NSListCB(this);
	cb->SetRedoPolicy(rp);
	WatcherAction* action		= new NsListWatcherAction(this);
	std::string	node_path		= GetRootPath() + NAME_SPACE_LIST_PATH; 
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

int ZkBrokerProxy::GetNsIdInt(RedoPolicy* rp, string& ns){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ZkBrokerProxy::GetNsIdInt() cur state  ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	CompletionCallBack * cb		= new GetNsIdIntCB(this, ns);
	cb->SetRedoPolicy(rp);
	WatcherAction* action		= new GetNsIdIntWatcherAction(this, ns);
	std::string	node_path		= GetRootPath() + NAME_SPACE_LIST_PATH + '/' + ns; 
	//bool ret = CurReactor()->GetChildrenList(node_path, cb, action, rp->GetRedoTimeInterval());
	bool ret = CurReactor()->GetNodeValue(node_path, cb, action, rp->GetRedoTimeInterval());
	if (ret == false){                                  
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"proxy:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		IncrItemNum();
		return 0;
	}
}


int ZkBrokerProxy::GetRolesChildrenList( RedoPolicy* rp, const string& farm_id ){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"current ZkBrokerProxy::GetRolesChildrenList() cur state  ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	CompletionCallBack * p = new ProxyRolesChildrenMapCB(this, farm_id);
	p->SetRedoPolicy(rp);
	WatcherAction* action = new ProxyRolesListWatcherAction(this, farm_id);
	string node_path = GetRootPath() + FARM_LIST_PATH + "/" + farm_id + ROLES_PATH;
	int ret = CurReactor()->GetChildrenList(node_path, p, action, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"proxy:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		IncrItemNum();
		return 0;
	}
}

int ZkBrokerProxy::GetRolesDesc( RedoPolicy* rp, const string& farm_id, const string& version_path ){
	CompletionCallBack * p = new ProxyGetVersionValueCb(this, farm_id, version_path);
	p->SetRedoPolicy(rp);
	WatcherAction* action = new ProxyVersionNodeWatcherAction(this, farm_id, version_path);
	string node_path = GetRootPath() + FARM_LIST_PATH + "/" + farm_id + ROLES_PATH + "/" +  version_path;
	int ret = CurReactor()->GetNodeValue(node_path, p, action, rp->GetRedoTimeInterval());
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



	



