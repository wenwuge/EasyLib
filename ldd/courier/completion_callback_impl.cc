
#include <glog/logging.h>
#include "completion_callback_impl.h"
#include "redo_policy.h"
#include "zkbroker.h"



namespace ldd{
namespace courier{

void ReplicaNodeCB::GetCompletion(const char * value, int value_len, 
									const struct Stat* stat){
    util::RWMutexLock wlock(&broker_->rwmtx_, true);
	string v(value, value_len);
	broker_->replica_size_ = atoi(v.data());
	LOG(INFO)<<"OK ReplicaNodeCB replica_size: "<<broker_->replica_size_
			<<" value:"<<v.data();
	broker_->DecrItemNum();
}
void ReplicaNodeCB::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"re_GetReplicaSize()";
	broker_->_GetReplicaSize(redo_policy_);
	broker_->DecrItemNum();
}
void ReplicaNodeCB::ErrorHandle(){
	LOG(ERROR)<<"_GetReplicaSize() ErrorHandle() ERROR";
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}


void EngineNodeCB::GetCompletion(const char * value, int value_len, 
									const struct Stat* stat){
    util::RWMutexLock wlock(&broker_->rwmtx_, true);
	broker_->engine_name_.assign(value, value_len);
	LOG(INFO)<<"OK EngineNodeCB engine_name:"<<broker_->engine_name_;
	broker_->DecrItemNum();
}
void EngineNodeCB::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"re_GetStoreEngine()";
	broker_->_GetStoreEngine(redo_policy_);
	broker_->DecrItemNum();
}
void EngineNodeCB::ErrorHandle(){
	LOG(ERROR)<<"_GetStoreEngine() ErrorHandle() ERROR";
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}


void GetMaxBinLogCB::GetCompletion(const char * value, int value_len, 
									const struct Stat* stat){
    util::RWMutexLock wlock(&broker_->rwmtx_, true);
	string v(value, value_len);
	broker_->binlog_max_ = atoi(v.data());
	LOG(INFO)<<"OK GetMaxBinLogCB binlog_max:"<<broker_->binlog_max_
        <<" value:"<<v.data();
	broker_->DecrItemNum();
}
void GetMaxBinLogCB::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"re_GetMaxBinLogFile()";
	broker_->_GetMaxBinLogFile(redo_policy_);
	broker_->DecrItemNum();
}
void GetMaxBinLogCB::ErrorHandle(){
	LOG(ERROR)<<" GetMaxBinLogCB::ErrorHandle() ERROR";
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}


void GetBucketSizeCB::GetCompletion(const char * value, int value_len, 
									const struct Stat* stat){
    util::RWMutexLock wlock(&broker_->rwmtx_, true);
	string	v(value, value_len);
	broker_->buckets_size_ = atoi(v.data());
	LOG(INFO)<<"OK GetBucketSizeCB buckets_size: "<<broker_->buckets_size_
        <<" value:"<<v.data();
	broker_->DecrItemNum();
}
void GetBucketSizeCB::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"re_GetBucketSize()";
	broker_->_GetBucketSize(redo_policy_);
	broker_->DecrItemNum();
}
void GetBucketSizeCB::ErrorHandle(){
	LOG(ERROR)<<"GetBucketSizeCB::ErrorHandle() ERROR";
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}

void BucketListCB::GetChildrenCompletion(const struct String_vector *strings){
	if (strings->data){
        util::RWMutexLock wlock(broker_->GetRwLockPtr(), true);
		broker_->bucket_collection_.clear();
		// parse the children node
		for (int32_t i=0; i<strings->count; i++){
			char* p = strings->data[i];
			string bucketid;		bucketid = p;
			broker_->bucket_collection_[bucketid] = "";   // need to set value
			DLOG(INFO)<<"OK proxy::BucketListCB::GetChildren BucketID:"<<bucketid;
			RedoPolicy* rp = broker_->redo_policy_->Copy();
			broker_->GetFarmIdOfBucketID(rp,  bucketid);
		}		
	}
	broker_->DecrItemNum();
}
void BucketListCB::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"proxy::reGetBucketIDList()";
	broker_->GetBucketIDList(redo_policy_);
	broker_->DecrItemNum();
}
void BucketListCB::ErrorHandle(){
	LOG(ERROR)<<"proxy::GetBucketIDList() ErrorHandle()";
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}

void GetFarmIdOfBucketIdCB::GetCompletion(const char * value, int value_len, 
										  const struct Stat* stat){
    util::RWMutexLock wlock(broker_->GetRwLockPtr(), true);
	// how can I get the key;
	broker_->bucket_collection_[bucketID_] = string(value, value_len);
	LOG(INFO)<<"OK proxy::GetFarmIdCB "<<" bucketID_:"<<bucketID_<<" farmid:"
        <<broker_->bucket_collection_[bucketID_];
	broker_->DecrItemNum();
}
void GetFarmIdOfBucketIdCB::Retry(RedoPolicy* redo_policy_){
	DLOG(WARNING)<<"proxy::reGetBucketIDList()";
	broker_->GetFarmIdOfBucketID(redo_policy_, bucketID_);
	broker_->DecrItemNum();
}
void GetFarmIdOfBucketIdCB::ErrorHandle(){
	LOG(ERROR)<<"proxy::GetFarmIdOfBucketIdCB() ErrorHandle() bucketid:"<<bucketID_;
	broker_->SetCourierErrorCode(kInvalidBaseParamInZk);
	broker_->DecrItemNum();
}

}//namespace courier
}//namespace ldd













