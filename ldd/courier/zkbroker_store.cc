
#include <glog/logging.h>
#include "rapidjson/document.h"		
#include "rapidjson/prettywriter.h"	
#include "rapidjson/filestream.h"			
#include "reactor.h"
#include "zkbroker_store.h"
#include "completion_callback_store.h"
#include "watcher_action_store.h"
#include "watcher_action_impl.h"
#include "redo_policy.h"


namespace ldd{
namespace courier{		


ZkBrokerStore::ZkBrokerStore(const std::string& home_host_port) : 
					home_host_port_(home_host_port),isLeader_(false), 
					leader_ship_notify_(NULL),roles_change_notify_(NULL),
					online_nodes_changed_notify_(NULL){

}

ZkBrokerStore::~ZkBrokerStore(){
	DLOG(INFO)<<"ZkBrokerStore::~ZkBrokerStore()";
	ZooKeeperReactor* reactor_ = CurReactor();
	if (reactor_){
		reactor_->Close();
	}
}

int ZkBrokerStore::LoadAppConfig(){
	RedoPolicy* rp = CurRedoPolicy()->Copy();
	int ret = GetNodeList(rp);  // host:port0--farm0
	if (ret != 0){
		LOG(ERROR)<<"ZooKeeperBroker GetReplicaSize ERROR, ret:"<<ret;
		return ret;
	}

	return 0;
}

int ZkBrokerStore::GetRoles(RolesChangedNotify cb, Roles* roles){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ZkBrokerStore::GetRoles() current state not right ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	if (!roles){
		DLOG(INFO)<<"ZkBrokerStore::GetRoles() - parameter ERROR";
		return kInvalidArgument;; // 参数错误
	}
	roles_change_notify_ = cb;
    util::RWMutexLock rlock(GetRwLockPtr());
	roles->master = cur_roles_st_.master;
	for (size_t i=0; i<cur_roles_st_.slaves.size(); i++){
		roles->slaves.push_back(cur_roles_st_.slaves[i]);
	}
	for (size_t i=0; i<cur_roles_st_.followers.size(); i++){
		roles->followers.push_back(cur_roles_st_.followers[i]);
	}

	return 0;
} 

int ZkBrokerStore::GetAliveNodes(OnlineNodesChangedNotify cb, std::vector<std::string>* server_list){
	DLOG(INFO)<<"ZkBrokerStore::GetAliveNodes()";
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ZkBrokerStore::GetOnlineNodes() current state not right ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	if (!server_list){ 
		DLOG(INFO)<<"ZkBrokerStore::GetOnlineNodes() - parameter ERROR";
		return kInvalidArgument;; // 参数错误
	}
	online_nodes_changed_notify_ = cb;
    util::RWMutexLock rlock(GetRwLockPtr());
	std::vector<std::string>::const_iterator itr = alive_node_collection_.begin();
	for (; itr != alive_node_collection_.end(); itr++){
		server_list->push_back(*itr);
		LOG(INFO)<<"Alive node="<<*itr;
	}
	return 0 ;
}

void ZkBrokerStore::FilterOwnBucketID(){
    util::RWMutexLock rlock(GetRwLockPtr()); 
	map<string, string>::const_iterator itr = bucket_collection_.begin();
	for (; itr!= bucket_collection_.end(); itr++){
		uint16_t bucket_id = 0;
		const string& bucket = itr->first;
		LOG(INFO)<<"ZkBrokerStore::FilterOwnBucketID() check new bucket= "<<bucket<<" "<<itr->second<<" =? "<<farm_id_;
		if (farm_id_.compare(itr->second) == 0){
			if (bucket.size() == 0){
				LOG(ERROR)<<"ZkBrokerStore::FilterOwnBucketID() convert error bucket= "<<bucket<<" farmid "<<itr->second;
				continue;
			}
			
			if (bucket[0]>= '0' && bucket[0]<= '9'){
				bucket_id = atoi(bucket.data());
				bucket_set_.insert(bucket_id);
				LOG(INFO)<<"ZkBrokerStore::FilterOwnBucketID() insert new bucketid= "<<bucket_id<<" farmid "<<itr->second;
			} //if '0' '9'	
		}//if
	}//for
}

std::set<uint16_t>&	 ZkBrokerStore::GetBucketSet(){ 
	return	    bucket_set_;
};

int ZkBrokerStore::StartLeaderElection(LeaderShipChangedNotify cb){
	if (CourierErrorCode() != 0 ){
	   LOG(ERROR)<<"ZkBrokerStore::StartLeaderElection() current state not right ERROR="<<CourierErrorCode();
	   return CourierErrorCode();
	}
	FilterOwnBucketID();
	leader_ship_notify_ = cb;

	DLOG(INFO)<<"StartLeaderElection() "<<"farmid:"<<farm_id_;
	/*RedoPolicy* rp = CurRedoPolicy()->Copy();
	int ret = CreateAliveNode(rp, home_host_port_);
	if (ret != 0){
	   LOG(ERROR)<<"ERROR store:: CreateAliveNode ERROR, ret:"<<ret;
	   return ret;
	}*/

    int ret = 0;
	ret = DoLeaderElection();
	if (ret != 0){
	   LOG(ERROR)<<"store:: DoLeaderElection ERROR, ret:"<<ret;
	   return ret;
	}

	return 0;
}

int ZkBrokerStore::DoLeaderElection(){
	//// 1. CHECK whether coordinator node exists
	cord_node_path_ = GetRootPath() + FARM_LIST_PATH + "/" + farm_id_ + COORDINATION;
	SetNodePrefix();
	LOG(INFO)<<"DoLeaderElection() start leader election in farm "<< farm_id_;
	RedoPolicy* rp = CurRedoPolicy()->Copy();
	int ret = CheckCordNodeExist(rp);
	if (ret != 0){
		LOG(ERROR)<<"store:: CheckCordNodeExist ERROR, ret:"<<ret;
		return ret;
	}

	return 0;
}

int ZkBrokerStore::AddNewRoles(const Roles& roles){
	// format sprintf(roles, "{  \"master\" : [\"host:port\"], \"slave\" : [\"host:port0\", \"host:port1\"], \"follower\" : [\"host:port\", \"host:port\"]}");
	string role_desc;

	role_desc += " { \"master\" : [\"";
	role_desc += roles.master.data();
	role_desc += "\"] ,\"slave\" : [\"";
	for (size_t i=0; i < roles.slaves.size(); i++){
		role_desc += roles.slaves[i];
		role_desc += "\"";
		if (i < roles.slaves.size() - 1){
			role_desc += ", \"";
		}
	}
	if (roles.slaves.size() == 0){
		role_desc += "\"";
	}
	
	role_desc += "], \"follower\" : [\"";
	for (size_t i=0; i < roles.followers.size(); i++){
		role_desc += roles.followers[i];
		role_desc += "\"";
		if (i < roles.followers.size() - 1){
			role_desc += ", \"";
		}
	}

	if (roles.followers.size() == 0){
		role_desc += "\"";
	}
	
	role_desc += "]}";

	DLOG(INFO)<<"AddNewRoles() roles_desc"<<role_desc;

	RedoPolicy* rp = CurRedoPolicy()->Copy();
	int ret = CreateVersionNode(rp, role_desc);
	if (ret != 0){
		LOG(ERROR)<<"store:: CreateVersionNode ERROR, ret:"<<ret;
		return ret;
	}
	return 0;
}

void ZkBrokerStore::CheckHomeHostPort(){
	std::map<std::string, std::string>::const_iterator itr;
	itr = node_collection_.find(home_host_port_);
	if (itr == node_collection_.end()){
		LOG(ERROR)<<"not exist this home_host_port="<<home_host_port_;
		SetCourierErrorCode(kInvalidHomeAddress);
		return ;
	}
}

void ZkBrokerStore::GetFarmIdPair(const std::string& host_port, const std::string& farm_id){
	if (home_host_port_.compare(host_port) == 0){
		farm_id_ = farm_id;
		LOG(INFO)<<"HIT!! host_port="<<host_port<<" farm_id="<<farm_id;

       
		if (CourierErrorCode() != 0 ){
			LOG(ERROR)<<"ZkBrokerStore::StartLeaderElection() current state not right ERROR="<<CourierErrorCode();
			return ;
		}

        int ret = 0;
        RedoPolicy* rp = NULL;
        //等到拿到farmid后再创建
        rp = CurRedoPolicy()->Copy();
        ret = CreateAliveNode(rp, home_host_port_);
        if (ret != 0){
            LOG(ERROR)<<"ERROR store:: CreateAliveNode ERROR, ret:"<<ret;
            return ;
        }

		
		rp = CurRedoPolicy()->Copy();
		ret = GetAliveNodeList(rp);
		if (ret != 0){
			LOG(ERROR)<<"ERROR store:: GetAliveNodeList ERROR, ret:"<<ret;
			return ;
		}

		rp = CurRedoPolicy()->Copy();
		ret = GetRolesChildrenList(rp);  //
		if (ret != 0){
			LOG(ERROR)<<"store:: GetAliveNodeList ERROR, ret:"<<ret;
			return ;
		}
	}
	LOG(INFO)<<"host_port="<<host_port<<" farm_id="<<farm_id;
}

void ZkBrokerStore::RolesChangedCB(std::string& roles_desc){
		Roles tmp;
	
		{
            util::RWMutexLock wlock(GetRwLockPtr(), true);
			cur_roles_st_.master.clear();
			cur_roles_st_.slaves.clear();
			cur_roles_st_.followers.clear();
			if (!GetRolesFromJson(roles_desc, &cur_roles_st_)){
				LOG(ERROR)<<"ZkBrokerStore::RolesChangedCB(), parse error";
				SetCourierErrorCode(kInvalidArgument);
				return;
			}

			tmp.master = cur_roles_st_.master;
			for (size_t i=0; i<cur_roles_st_.slaves.size(); i++){
				tmp.slaves.push_back(cur_roles_st_.slaves[i]);
			}
			for (size_t i=0; i<cur_roles_st_.followers.size(); i++){
				tmp.followers.push_back(cur_roles_st_.followers[i]);
			}

			tmp = cur_roles_st_;
			DLOG(INFO)<<"RolesChangedCB() get ,roles_desc:" <<roles_desc;
		}

		if (roles_change_notify_){
			DLOG(INFO)<<"RolesChangedCB() - RolesChangedNotify,roles_desc:" <<roles_desc;
			roles_change_notify_(CourierErrorCode(), tmp);
		}
}

void ZkBrokerStore::CheckCordChildrenList(){
	LookupNode();
	if (node_id_under_cord_.empty()){ // create node_id_under_cord_
		RedoPolicy* rp = CurRedoPolicy()->Copy();
		int ret = CreateNodeUnderCord(rp);
		if (ret != 0){
			LOG(ERROR)<<"store CreateNodeUnderCord ERROR, ret:"<<ret;
			return;
		}
	}else{
		// check node_id_under_cord_ is the leader
		precedor_name_ = ChildFloor();
		if (precedor_name_.empty()){
			if (cur_leader_.compare(node_id_under_cord_)== 0 ){
				isLeader_ = true;
				LOG(INFO)<<"CheckCordChildrenList() I'am the leader!!! node_id="<<node_id_under_cord_;
				if (leader_ship_notify_){
					LOG(INFO)<<"leader_ship_notify_ I'am the leader!!! notify";
					leader_ship_notify_(CourierErrorCode(), true);
				}
			}
		}else{
			LOG(INFO)<<"CheckCordChildrenList() I'am not leader!!! Watch precedor node; cur leader="<<cur_leader_;
			int ret = CheckPrecedorExist();
			if (ret != 0){
				LOG(ERROR)<<"store CreateNodeUnderCord ERROR, ret:"<<ret;
				return;
			}
		}
	}
}

void ZkBrokerStore::SetNodePrefix(){
	// get the session id
	int64_t session = CurReactor()->ClientId();
	char prefix[30];
	int len = 0;
#if defined(__x86_64__)
	len = snprintf(prefix, 30, "x-%016lx-", session);
#else 
	len= snprintf(prefix, 30, "x-%016llx-", session);
#endif
	node_prefix_.assign(prefix, len);
	LOG(INFO)<<"SetNodePrefix() competing node's prefix"<<node_prefix_;
}

/** see if our node already exists
* if it does then we dup the name and 
* return it
*/
void ZkBrokerStore::LookupNode() {
	node_id_under_cord_.clear();
	for (size_t i=0; i < cord_collection_.size(); i++){
		if (strncmp(node_prefix_.data(),
			cord_collection_[i].data(), node_prefix_.length()) == 0){
			node_id_under_cord_ = cord_collection_[i];
			LOG(INFO)<<"LookupNode() currrent competing node path: "<<node_id_under_cord_;
			break;
		}
	}
}

static int vstrcmp(const void* str1, const void* str2) {
	const char **a = (const char**)str1;
	const char **b = (const char**) str2;
	return strcmp(strrchr(*a, '-')+1, strrchr(*b, '-')+1); 
} 

void ZkBrokerStore::SortChildren(char ** data, int32_t count) {
	qsort(data, count, sizeof(char*), &vstrcmp);
}

string ZkBrokerStore::ChildFloor(){
	string ret;
	for (size_t i=0; i < cord_collection_.size(); i++){
		const char* a = strrchr(cord_collection_[i].data(), '-');
		const char* b = strrchr(node_id_under_cord_.data(), '-');
		if (a == NULL || b == NULL){
			continue;
		}
		if (strcmp(a, b) < 0){
			ret = cord_collection_[i] ;
		}
	}
	LOG(INFO)<<"ChildFloor precedor node:"<<ret<<" NULL";
	return ret;
}


/************************************************************************/
/*these functions below need set callback and watcher					*/
/************************************************************************/

/************************************************************************/
/*    for leader_election not for conf, so  no need IncrItemNum()       */
/************************************************************************/
int ZkBrokerStore::CheckCordNodeExist( RedoPolicy* rp ){   
	//check cord == coordinator
	CompletionCallBack* cb = new CoordinatorNodeExistsCB(this);
	cb->SetRedoPolicy(rp);
	bool ret = CurReactor()->Exists(cord_node_path_, cb, 
									NULL, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"store:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		return 0;
	}
}

// get cord node children list
int ZkBrokerStore::GetCordNodeList( RedoPolicy* rp ){
	CompletionCallBack* cb = new CordChildrenListCB(this);
	cb->SetRedoPolicy(rp);
	bool ret = CurReactor()->GetChildrenList(cord_node_path_, cb,
										NULL, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"store:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		return 0;
	}
}

int ZkBrokerStore::CreateNodeUnderCord( RedoPolicy* rp ){
	CompletionCallBack* cb = new CreateNodeUnderCordCB(this);
	cb->SetRedoPolicy(rp);
	string value;
	string node_path = cord_node_path_ + "/" + node_prefix_; 
	bool ret = CurReactor()->CreateNode(node_path, value,
						 ZOO_EPHEMERAL|ZOO_SEQUENCE, cb, NULL, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"store:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		return 0;
	}
}

int ZkBrokerStore::CheckPrecedorExist(){
	string path = cord_node_path_ + "/" + precedor_name_;
	WatcherAction* wa = new PrecedorExistWA(this);
	bool ret = CurReactor()->Exists(path, NULL, wa, 0);
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"store:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		return 0;
	}
}

int ZkBrokerStore::CreateVersionNode( RedoPolicy* rp, string & roles_desc ){
	CompletionCallBack* cb = new CreateVersionNodeCB(this, roles_desc);
	cb->SetRedoPolicy(rp);
	string version = GetRootPath() + FARM_LIST_PATH + "/" + farm_id_ + ROLES_PATH + VERSION_PATH;
	bool ret = CurReactor()->CreateNode(version, roles_desc, ZOO_SEQUENCE, 
										cb, NULL, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"store:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		return 0;
	}
}


int ZkBrokerStore::CreateAliveNode( RedoPolicy* rp, const string& host_port ){
	CompletionCallBack * cb = new CreateAliveNodeCB(this, host_port);
	cb->SetRedoPolicy(rp);
	string node_path = GetRootPath() + FARM_LIST_PATH  + "/" + farm_id_ 
		+ ALIVE_NODES_LIST_PATH + "/" + host_port;
	string value;
	bool ret = CurReactor()->CreateNode(node_path, value, ZOO_EPHEMERAL, 
		cb, NULL, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"store:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
        IncrItemNum();
		return 0;
	}
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

int  ZkBrokerStore::GetNodeList( RedoPolicy* rp ){
	CompletionCallBack * p = new NodesMapCB(this);
	p->SetRedoPolicy(rp);
	WatcherAction* action = new NodeListWatcherAction(this);
	std::string	node_path		= GetRootPath() + NODE_LIST_PATH; 
	bool ret = CurReactor()->GetChildrenList(node_path, p, action, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"store:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		IncrItemNum();
		return 0;
	}
}

int ZkBrokerStore::GetFarmIdOfNode( RedoPolicy* rp, const std::string& host_port ){
	CompletionCallBack * p = new GetFarmIdOfNodeCb(this, host_port);
	p->SetRedoPolicy(rp);
	WatcherAction* action = new NodeListWatcherAction(this);
	std::string	node_path		= GetRootPath() + NODE_LIST_PATH + "/" + host_port; 
	bool ret = CurReactor()->GetNodeValue(node_path, p, action, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"store:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		IncrItemNum();
		return 0;
	}
}

int ZkBrokerStore::GetAliveNodeList( RedoPolicy* rp ){
	CompletionCallBack * p = new AliveNodeMapCB(this);
	WatcherAction* action = new AliveNodeListWatcherAction(this);
	p->SetRedoPolicy(rp);
	string node_path = GetRootPath() + FARM_LIST_PATH + "/" + farm_id_ + ALIVE_NODES_LIST_PATH;
	bool ret = CurReactor()->GetChildrenList(node_path, p, action, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"store:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		IncrItemNum();
		return 0;
	}
}

int ZkBrokerStore::GetRolesChildrenList( RedoPolicy* rp ){
	CompletionCallBack * p = new VersionNodeMapCB(this, cur_version_path_);
	p->SetRedoPolicy(rp);
	WatcherAction* action = new RolesListWatcherAction(this);
	string node_path = GetRootPath() + FARM_LIST_PATH + "/" + farm_id_ + ROLES_PATH;
	bool ret = CurReactor()->GetChildrenList(node_path, p, action, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"store:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		IncrItemNum();
		return 0;
	}
}

int ZkBrokerStore::GetRolesDesc( RedoPolicy* rp, const string& version_path ){
	CompletionCallBack * p = new GetVersionValueCb(this, version_path);
	p->SetRedoPolicy(rp);
	WatcherAction* action = new VersionNodeWatcherAction(this, version_path);
	string node_path = GetRootPath() + FARM_LIST_PATH + "/" + 
								farm_id_ + ROLES_PATH + "/" +  version_path;
	bool ret = CurReactor()->GetNodeValue(node_path, p, action, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"store:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		IncrItemNum();
		return 0;
	}
}

int ZkBrokerStore::DeleteRolesDesc( const string& version_path ){
    //CompletionCallBack * p = new GetVersionValueCb(this, version_path);
    //p->SetRedoPolicy(rp);
    //WatcherAction* action = new VersionNodeWatcherAction(this, version_path);
    string node_path = GetRootPath() + FARM_LIST_PATH + "/" + 
        farm_id_ + ROLES_PATH + "/" +  version_path;
    bool ret = CurReactor()->DeleteNode(node_path, -1, NULL, NULL, 0);
    if (ret == false){
        SetCourierErrorCode(kZooKeeperNotWork);
        LOG(ERROR)<<"store:: SetZkErrorCode ERROR, kZkNotWork";
        return kZooKeeperNotWork; 
    }else{
        return 0;
    }
}



}//namespace courier
}//namespace ldd








