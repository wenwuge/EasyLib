
#include <glog/logging.h>
#include "completion_callback_store.h"
#include "redo_policy.h"
#include "zkbroker_store.h"



namespace ldd{
namespace courier{
/************************************************************************/
/* for zkfarm_broker    leader election                                 */
/************************************************************************/
void CoordinatorNodeExistsCB::ExistsCompletion(const struct Stat *stat){
	LOG(INFO)<<"store::coordinator node exists, CoordinatorNodeExists";
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	//broker_->CreateNodeUnderCord(rp);
	broker_->GetCordNodeList(rp);
}
void CoordinatorNodeExistsCB::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"store::reCheckCordNodeExist()";
	broker_->CheckCordNodeExist(redo_policy_); // retry
}
void CoordinatorNodeExistsCB::ErrorHandle(){
	LOG(ERROR)<<"proxy::CoordinatorNodeExistsCB() not existed ErrorHandle()";
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
}

void CreateNodeUnderCordCB::CreateCompletion(const char* value){
	if (value){
		// should get the last name of the path
		const char* name = strrchr(value, '/');
		if (name){
			broker_->node_id_under_cord_ = string(name+1);
			DLOG(INFO)<<"OK store::CreateNodeUnderCordCB(), fullpah:"<<value<<" path:"<<name;
			// /farms/farm0/coordinator/x-013ed52fc55c0074-0000000271 path:/x-013ed52fc55c0074-0000000271 example
			RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
			broker_->GetCordNodeList(rp);
		}
	}
}
void CreateNodeUnderCordCB::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"store::Retry reCreateNodeUnderCord()";
	broker_->CreateNodeUnderCord(redo_policy_);
}
void CreateNodeUnderCordCB::ErrorHandle(){
	LOG(ERROR)<<"proxy::CreateNodeUnderCordCB() including already exist ErrorHandle()";
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
}


void CordChildrenListCB::GetChildrenCompletion(const struct String_vector *strings){
	if (!strings->data){
		return;
	}
	broker_->SortChildren(strings->data, strings->count);
	broker_->cord_collection_.clear();
	for (int32_t i=0; i<strings->count; i++){
		char* p = strings->data[i];
		string key;		key = p;
		broker_->cord_collection_.push_back(key);
		DLOG(INFO)<<"OK store::CordChildrenListCB cord children key:"<<key;
	}// 
	if (broker_->cord_collection_.size() > 0){
		broker_->cur_leader_ = broker_->cord_collection_[0];
		LOG(INFO)<<"store::CordChildrenListCB Leader key:"<<broker_->cur_leader_;
	}
	broker_->CheckCordChildrenList();
}
void CordChildrenListCB::Retry(RedoPolicy* redo_policy_){
	// retry   to do  set a retry policy
	DLOG(WARNING)<<"store::Retry reGetCordNodeList()";
	broker_->GetCordNodeList(redo_policy_); // retry
}
void CordChildrenListCB::ErrorHandle(){
	LOG(ERROR)<<"proxy::CordChildrenListCB() ErrorHandle()";
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
}

void CreateVersionNodeCB::CreateCompletion(const char* value){
	if (value){
		DLOG(INFO)<<"OK store::CreateVersionNodeCB, fullpah:"<<value;
	}
}
void CreateVersionNodeCB::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"store::Retry reCreateVersionNode()£¬ roles_desc:"<<roles_desc_;
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->CreateVersionNode(rp, roles_desc_);
}
void CreateVersionNodeCB::ErrorHandle(){
	LOG(ERROR)<<"proxy::CreateVersionNodeCB() ErrorHandle() roles_desc:"<<roles_desc_;
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
}

void CreateAliveNodeCB::CreateCompletion(const char* value){
	if (value){
		DLOG(INFO)<<"OK store::CreateAliveNodeCB, fullpah:"<<value;
    }else{
        DLOG(ERROR)<<"ERROR store::CreateAliveNodeCB, NULL value:";
    }
    broker_->DecrItemNum();
}
void CreateAliveNodeCB::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"store::CreateAliveNodeCB reCreateAliveNode() host_port:"<<host_port_;
	broker_->CreateAliveNode( redo_policy_,  host_port_); 
    broker_->DecrItemNum();
}
void CreateAliveNodeCB::ErrorHandle(){
	LOG(ERROR)<<"store:: CreateAliveNodeCB() ErrorHandle() host_port:"<<host_port_;
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
    broker_->DecrItemNum();
}


/************************************************************************/
/* Get base conf                                                        */
/************************************************************************/
void NodesMapCB::GetChildrenCompletion(const struct String_vector *strings){
	if (strings->data){
        util::RWMutexLock wlock(broker_->GetRwLockPtr(), true);
		// parse the children node
		for (int32_t i=0; i<strings->count; i++){
			char* p = strings->data[i];
			string host_port;		host_port = p;
			string farm_id;
			broker_->node_collection_[host_port] = farm_id;
			DLOG(INFO)<<"OK store::NodesMapCB GetChildren() node:"<<host_port;
			RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
			broker_->GetFarmIdOfNode(rp, host_port);
		}
		broker_->CheckHomeHostPort();
	}
	broker_->DecrItemNum();
}
void NodesMapCB::Retry(RedoPolicy* redo_policy_){
	// retry   to do  set a retry policy
	DLOG(WARNING)<<"store:: reGetNodeList()";
	broker_->GetNodeList(redo_policy_);
	broker_->DecrItemNum();
}
void NodesMapCB::ErrorHandle(){
	LOG(ERROR)<<"store:: NodesMapCB() ErrorHandle()";
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}
void GetFarmIdOfNodeCb::GetCompletion(const char * value, int value_len, 
								const struct Stat* stat){
    util::RWMutexLock wlock(broker_->GetRwLockPtr(), true);
	// how can I get the key;
	broker_->node_collection_[host_port_] = string(value, value_len);
	broker_->GetFarmIdPair(host_port_, broker_->node_collection_[host_port_]);
	DLOG(INFO)<<"OK store::GetFarmIdOfNodeCb GetCompletion() node="<<host_port_
		<<" farmid="<<broker_->node_collection_[host_port_]; 
	broker_->DecrItemNum();
}
void GetFarmIdOfNodeCb::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"store::reGetFarmIdOfNode(), host_port:"<<host_port_;
	broker_->GetFarmIdOfNode(redo_policy_,  host_port_);
	broker_->DecrItemNum();
}
void GetFarmIdOfNodeCb::ErrorHandle(){
	LOG(ERROR)<<"store:: GetFarmIdOfNodeCb() ErrorHandle() host_port:"<<host_port_;
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}


void AliveNodeMapCB::GetChildrenCompletion(const struct String_vector *strings){
	vector<string> new_list;
	// parse the children node
	if (strings->data){
		for (int32_t i=0; i<strings->count; i++){
			char* p = strings->data[i];
			string key;		key = p;
			new_list.push_back(key);
			DLOG(INFO)<<"OK store::AliveNodeMapCB::GetChildren(), total size="
				<<strings->count<<" NodeKey:"<<key;

			bool added =true;
			for (size_t i=0; i<broker_->alive_node_collection_.size(); i++){
				if (key.compare(broker_->alive_node_collection_[i]) == 0){
					added = false;
					break;
				}
			}
			if (added && broker_->online_nodes_changed_notify_){
				LOG(INFO)<<"store::AliveNodeMapCB::GetChildren notify when new node of path="<<key;
				broker_->online_nodes_changed_notify_(0, kCreateNode, key);
			}
		}	
	}
	// check node created or deleted
	for (size_t i=0; i<broker_->alive_node_collection_.size(); i++){
		bool deleted = true;
		for(size_t j=0; j<new_list.size(); j++){
			if (broker_->alive_node_collection_[i].compare(new_list[j]) == 0 ){
				deleted =false;
				break;
			}
		}
		if (deleted && broker_->online_nodes_changed_notify_){
			LOG(INFO)<<"store::notify when delete node of path:"<<broker_->alive_node_collection_[i];
			broker_->online_nodes_changed_notify_(0, kDeleteNode, broker_->alive_node_collection_[i]);
		}
	}
	// alive_node_collection_ only be modify here, so no need to get read lock when reading(above operation)
	{
        util::RWMutexLock wlock(broker_->GetRwLockPtr(), true);
		broker_->alive_node_collection_.clear();
		broker_->alive_node_collection_ = new_list;
	}
	broker_->DecrItemNum();
}
void AliveNodeMapCB::Retry(RedoPolicy* redo_policy_){
	// retry   to do  set a retry policy
	DLOG(WARNING)<<"store::reGetAliveNodeList()";
	broker_->GetAliveNodeList(redo_policy_);
	broker_->DecrItemNum();
}
void AliveNodeMapCB::ErrorHandle(){
	LOG(ERROR)<<"store:: AliveNodeMapCB() ErrorHandle()";
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}

void VersionNodeMapCB::GetChildrenCompletion(const struct String_vector *strings){
	if (strings->data){
		broker_->SortRolesVersionChildren(strings->data, strings->count);
		//parse the children node
		LOG(INFO)<<"OK store::VersionNodeMapCB GetChildren total_size: "<<strings->count;
		if (strings->count > 0){
			char* p = strings->data[strings->count - 1];
			string version_path;		version_path = p;
            LOG(INFO)<<"cur_version_path_="<<cur_version_path_<<" last path="<<version_path;
            if (cur_version_path_.compare(version_path) != 0){
                LOG(INFO)<<"OK store::VersionNodeMapCB GetChildren just get last node:"<<version_path;
                RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
                LOG(INFO)<<"OK store::GetRolesDesc of node:"<<version_path;
                cur_version_path_ = version_path;
                broker_->GetRolesDesc(rp, version_path);
            }  
		}

        if (strings->count > HISTORY_ROLES_NUMBER){
            for (int i=0; i<strings->count - HISTORY_ROLES_NUMBER; i++){
                char* p = strings->data[i];
                string version_path;		version_path = p;
                LOG(INFO)<<"OK store::VersionNodeMapCB  delete history:"<<version_path;
                broker_->DeleteRolesDesc(version_path);
            }
        }
	}
	broker_->DecrItemNum();
}
void VersionNodeMapCB::Retry(RedoPolicy* redo_policy_){
	// retry   to do  set a retry policy
	DLOG(WARNING)<<"store::reGetRolesChildrenList()";
	broker_->GetRolesChildrenList(redo_policy_);
	broker_->DecrItemNum();
}
void VersionNodeMapCB::ErrorHandle(){
	LOG(ERROR)<<"store:: VersionNodeMapCB() ErrorHandle()";
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}


void GetVersionValueCb::GetCompletion(const char * value, int value_len, 
							const struct Stat* stat){
	// how can I get the key;
	std::string roles_desc(value, value_len);
	DLOG(INFO)<<"OK store::GetVersionValueCb node:"<<version_path_<<" rolesdesc"<<roles_desc; 
	//if (broker_->roles_change_notify_){
		broker_->RolesChangedCB(roles_desc);
	//}
	broker_->DecrItemNum();
}
void GetVersionValueCb::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"store::reGetRolesDesc(), version_path:"<<version_path_;
	broker_->GetRolesDesc(redo_policy_,  version_path_);
	broker_->DecrItemNum();
}
void GetVersionValueCb::ErrorHandle(){
	LOG(ERROR)<<"store:: GetVersionValueCb() ErrorHandle() version_path:"<<version_path_;
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}

}//namespace courier
}//namespace ldd













