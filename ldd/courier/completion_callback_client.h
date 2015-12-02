#ifndef __LDD_COURIER_COMPLETION_CALLBACK_CLIENT_H_
#define __LDD_COURIER_COMPLETION_CALLBACK_CLIENT_H_

#include "completion_callback.h"
#include "zkbroker_client.h"



namespace ldd{
namespace courier{


class ClientNodeListCB : public CompletionCallBack{
public:
	ClientNodeListCB(ZkBrokerClient* b): broker_(b){
	};
	virtual void GetChildrenCompletion(const struct String_vector *strings){
		vector<string> new_list;
		// broker_->node_collection_.clear();
		// parse the children node
		if (strings->data){
			DLOG(INFO)<<"ok ProxyNodeListCB::GetChildren strings->count: "<<strings->count;
			for (int32_t i=0; i<strings->count; i++){
				char* p = strings->data[i];
				string key;		key = p;
				new_list.push_back(key);
				DLOG(INFO)<<"OK proxy::ProxyNodeListCB::GetChildren NodeKey:"<<key;

				bool added =true;
				for (size_t i=0; i<broker_->node_collection_.size(); i++){
					if (key.compare(broker_->node_collection_[i]) == 0 ){
						added = false;
						break;
					}
				}
				if (added && broker_->node_list_notify_){
					LOG(INFO)<<"proxy::ProxyNodeListCB::GetChildren notify when new node of path:"<<key;
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
			DLOG(INFO)<<"GetChildrenCompletion() node_collection size()="<<new_list.size();
			broker_->node_collection_ = new_list;
		}

		broker_->DecrItemNum();
	}
	virtual void Retry(RedoPolicy* redo_policy_){
		DLOG(WARNING)<<"proxy::reGetReplicaSize()";
		broker_->GetProxyList(redo_policy_);
		broker_->DecrItemNum();
	}
	virtual void ErrorHandle(){
		LOG(ERROR)<<"proxy::GetNodeList() ErrorHandle() ";
		broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
		broker_->DecrItemNum();
	}
private:
	ZkBrokerClient* broker_;
};


}//namespace courier
}//namespace ldd



#endif








