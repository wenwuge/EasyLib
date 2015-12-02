
#include <glog/logging.h>
#include "completion_callback_proxy.h"
#include "redo_policy.h"
#include "zkbroker_proxy.h"



namespace ldd{
namespace courier{


void NodeListCB::GetChildrenCompletion(const struct String_vector *strings){
	vector<string> new_list;
	// broker_->node_collection_.clear();
	// parse the children node
	if (strings->data){
		DLOG(INFO)<<"ok NodeListCB::GetChildren strings->count: "<<strings->count;
		for (int32_t i=0; i<strings->count; i++){
			char* p = strings->data[i];
			string key;		key = p;
			new_list.push_back(key);
			DLOG(INFO)<<"OK proxy::NodeListCB::GetChildren NodeKey:"<<key;

			bool added =true;
			for (size_t i=0; i<broker_->node_collection_.size(); i++){
				if (key.compare(broker_->node_collection_[i]) == 0 ){
					added = false;
					break;
				}
			}
			if (added && broker_->node_list_notify_){
				LOG(INFO)<<"proxy::NodeListCB::GetChildren notify when new node of path:"<<key;
				broker_->node_list_notify_(0, kCreateNode, key);
			}
		}	
	}
	// check node created or deleted
	for (size_t i=0; i<broker_->node_collection_.size(); i++){
		bool deleted = true;
		for(size_t j=0; j<new_list.size(); j++){
			if (broker_->node_collection_[i].compare(new_list[j]) == 0){
				deleted =false;
				break;
			}
		}
		if (deleted && broker_->node_list_notify_){
			LOG(INFO)<<"proxy::notify when delete node of path:"<<broker_->node_collection_[i];
			broker_->node_list_notify_(0, kDeleteNode, broker_->node_collection_[i]);
		}
	}
	// node_collection_ only be modify here, so no need to get read lock when reading(above operation)
	{
        util::RWMutexLock wlock(broker_->GetRwLockPtr(), true);
		broker_->node_collection_.clear();
		DLOG(INFO)<<"insert new_list to node_collection size()"<<new_list.size();
		broker_->node_collection_ = new_list;
	}

	broker_->DecrItemNum();
}
void NodeListCB::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"proxy::reGetReplicaSize()";
	broker_->GetNodeList(redo_policy_);
	broker_->DecrItemNum();
}
void NodeListCB::ErrorHandle(){
	LOG(ERROR)<<"proxy::GetNodeList() ErrorHandle() ";
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}

void CreateProxyNodeCB::CreateCompletion(const char* value){
    if (value){
        DLOG(INFO)<<"OK proxy::CreateProxyNodeCB, fullpah:"<<value;
    }
    broker_->DecrItemNum();
}
void CreateProxyNodeCB::Retry(RedoPolicy* redo_policy_){
    DLOG(WARNING)<<"proxy::CreateProxyNodeCB reCreateAliveNode() ";
    broker_->CreateProxyNode( redo_policy_);  
    broker_->DecrItemNum();
}
void CreateProxyNodeCB::ErrorHandle(){
    LOG(ERROR)<<"proxy:: CreateProxyNodeCB() ErrorHandle() ";
    broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
    broker_->DecrItemNum();
}

void NSListCB::GetChildrenCompletion(const struct String_vector *strings){
	if (strings->data){
        util::RWMutexLock wlock(broker_->GetRwLockPtr(), true);
		broker_->ns_collection_.clear();
		// parse the children node
		for (int32_t i=0; i<strings->count; i++){
			char* p = strings->data[i];
			string ns;		ns = p;
			broker_->ns_collection_[ns] = -1;   // need to set value
			DLOG(INFO)<<"OK proxy::NSListCB::GetChildren ns:"<<ns;
			RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
			broker_->GetNsIdInt(rp, ns);
		}		
	}
	broker_->DecrItemNum();
}
void NSListCB::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"proxy::NSListCB::reGetNsIdList()";
	broker_->GetNsIdList(redo_policy_);
	broker_->DecrItemNum();
}
void NSListCB::ErrorHandle(){
	LOG(ERROR)<<"proxy::NSListCB::GetNameSpaceID() ErrorHandle()";
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}

void GetNsIdIntCB::GetCompletion(const char * value, int value_len, 
								 const struct Stat* stat){
    util::RWMutexLock wlock(broker_->GetRwLockPtr(), true);
	 // how can I get the key;
	 string tmp(value, value_len);
	 if (tmp[0]>='0' && tmp[0]<='9'){
		broker_->ns_collection_[ns_] = atoi(tmp.data());
	 }else{
		 LOG(ERROR)<<"GetNsIdIntCB::GetCompletion() error value not digit="<<tmp;
	 }
	 
	 LOG(INFO)<<"OK proxy::GetNsIdIntCB "<<" ns:"<<ns_<<" ns_int:"<<broker_->ns_collection_[ns_];
	 broker_->DecrItemNum();
}
void GetNsIdIntCB::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"proxy::reGetNsIdInt()";
	broker_->GetNsIdInt(redo_policy_, ns_);
	broker_->DecrItemNum();
}
void GetNsIdIntCB::ErrorHandle(){
	LOG(ERROR)<<"proxy::GetNsIdInt() ErrorHandle() bucketid:"<<ns_;
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}

void FarmListCB::GetChildrenCompletion(const struct String_vector *strings){
	if (strings->data){
        util::RWMutexLock wlock(broker_->GetRwLockPtr(), true);
		broker_->farm_roles_map_.clear();
		broker_->farm_collection_.clear();

		// parse the children node
		for (int32_t i=0; i<strings->count; i++){
			char* p = strings->data[i];
			string farm_id;		farm_id = p;

			int id = 0;
			if (p[0]>= '0' && p[0]<= '9'){
				id = atoi(p);
				broker_->farm_collection_.push_back(id);
				LOG(INFO)<<"FarmListCB::GetChildrenCompletion() add farm_id="<<id;
			}else{
				 LOG(ERROR)<<"FarmListCB::GetChildrenCompletion() error value not digit="<<p;
			}
			
			RedoPolicy* rp = broker_->redo_policy_->Copy();
			DLOG(INFO)<<"OK proxy::FarmListCB::GetChildren farm_id:"<<farm_id;
			rp = broker_->redo_policy_->Copy();
			DLOG(INFO)<<"proxy::FarmListCB::GetChildren get roles of farmid: "<<farm_id;
			broker_->GetRolesChildrenList(rp, farm_id);
		}
	}
	broker_->DecrItemNum();
}
void FarmListCB::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"proxy::reGetFarmList()";
	broker_->GetFarmList(redo_policy_);
	broker_->DecrItemNum();
}
void FarmListCB::ErrorHandle(){
	LOG(ERROR)<<"proxy::FarmListCB() ErrorHandle() ";
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}

void ProxyRolesChildrenMapCB::GetChildrenCompletion(const struct String_vector *strings){
	if (strings->data){
        util::RWMutexLock wlock(broker_->GetRwLockPtr(), true);
		broker_->SortRolesVersionChildren(strings->data, strings->count);

		DLOG(INFO)<<"OK proxy::ProxyRolesChildrenMapCB GetChildren, farmid="
                    <<farm_id_<<" total_size: "<<strings->count;
		if (strings->count > 0){
			char* p = strings->data[strings->count - 1];
			string version_path;		version_path = p;
			DLOG(INFO)<<"OK proxy::ProxyRolesChildrenMapCB GetChildren just get last node:"
                    <<"farmid="<<farm_id_<<" version="<<version_path
                    <<" cur_version_path="<<cur_version_path_;

            if (cur_version_path_.compare(version_path) != 0){
                RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
                DLOG(INFO)<<"OK GetRolesDesc of node:"<<version_path;
                cur_version_path_ = version_path;
                broker_->GetRolesDesc(rp, farm_id_, version_path);
            }  // cur_version_path_ always == "", because this cb object will be delete;
		}
	}
	broker_->DecrItemNum();
}
void ProxyRolesChildrenMapCB::Retry(RedoPolicy* redo_policy_){
	// retry   to do  set a retry policy
	DLOG(WARNING)<<"proxy::reGetRolesChildrenList(), farm_id:"<<farm_id_;
	broker_->GetRolesChildrenList(redo_policy_,  farm_id_);
	broker_->DecrItemNum();
}
void ProxyRolesChildrenMapCB::ErrorHandle(){
	LOG(ERROR)<<"proxy::ProxyRolesChildrenMapCB() ErrorHandle(), farm_id:"<<farm_id_;
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}

void ProxyGetVersionValueCb::GetCompletion(const char * value, int value_len, 
									  const struct Stat* stat){
    util::RWMutexLock wlock(broker_->GetRwLockPtr(), true);
    // how can I get the key;
    std::string roles_desc(value, value_len);
    Roles roles_st;
    DLOG(INFO)<<"proxy::GetRolesFromJson()"<<roles_desc;
    broker_->GetRolesFromJson(roles_desc, &roles_st);
    int nfarm =0 ;
    if (farm_id_[0]>= '0' && farm_id_[0]<= '9'){
        nfarm = atoi(farm_id_.data());
        broker_->farm_roles_map_[nfarm] = roles_st;
        DLOG(INFO)<< "insert farmid-roles farmid"<<nfarm<<" strfarmid= "<<farm_id_;
    }

    DLOG(INFO)<<"OK ProxyGetVersionValueCb node:"<<version_path_<<" farmid: "
        <<farm_id_<<" rolesdesc"<<roles_desc; 
    broker_->DecrItemNum();
}
void ProxyGetVersionValueCb::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"reGetRolesDesc() farm_id:"<<farm_id_<<" version_path_"<<version_path_;
	broker_->GetRolesDesc(redo_policy_,  farm_id_, version_path_);
	broker_->DecrItemNum();
}
void ProxyGetVersionValueCb::ErrorHandle(){
	LOG(ERROR)<<"proxy::ProxyGetVersionValueCb() ErrorHandle() farm_id:"
        <<farm_id_<<" version_path_"<<version_path_;
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}

}//namespace courier
}//namespace ldd













