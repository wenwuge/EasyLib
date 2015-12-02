#include <glog/logging.h>
#include "watcher_action_impl.h"
#include "redo_policy.h"
#include "zkbroker.h"



namespace ldd{
namespace courier{


//  param path znode path for which the watcher is triggered. NULL if the event 
void ReplicaNodeWatcherAction::onNodeValueChanged(zhandle_t*,const char* path){
	if (path){
		LOG(INFO)<<"ReplicaNodeWatcherAction node value changed of path:"<<path;
		LOG(INFO)<<"re_GetReplicaSize)";
	}
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->_GetReplicaSize(rp);
}

//  param path znode path for which the watcher is triggered. NULL if the event 
void StoreEngineNodeWatcherAction::onNodeValueChanged(zhandle_t*,const char* path){
	if (path){
		LOG(INFO)<<"StoreEngineNodeWatcherAction node value changed of path:"<<path;
		LOG(INFO)<<"reGetStoreEngine()";
	}
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->_GetStoreEngine(rp);
}

//  param path znode path for which the watcher is triggered. NULL if the event 
void BinLogNodeWatcherAction::onNodeValueChanged(zhandle_t*,const char* path){
	if (path){
		LOG(INFO)<<"BinLogNodeWatcherAction node value changed of path:"<<path;
		LOG(INFO)<<"reGetMaxBinLogFile()";
	}
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->_GetMaxBinLogFile(rp);
}

//  param path znode path for which the watcher is triggered. NULL if the event 
void BucketSizeNodeWatcherAction::onNodeValueChanged(zhandle_t*,const char* path){
	if (path){
		LOG(INFO)<<"BucketSizeNodeWatcherAction node value changed of path:"<<path;
		LOG(INFO)<<"reGetBucketSize()";
	}
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->_GetBucketSize(rp);
}


}//namespace courier
}//namespace ldd
















