#include <glog/logging.h>
#include "watcher_action_proxy.h"
#include "redo_policy.h"
#include "zkbroker_proxy.h"



namespace ldd{
namespace courier{

void PnodeListWatcherAction::onChildChanged(zhandle_t*,const char* path){
	if (path){
		LOG(INFO)<<"PnodeListWatcherAction children changed under path:"<<path;
		LOG(INFO)<<"reGetNodeList()";
	}
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->GetNodeList(rp);  
}


//  param path znode path for which the watcher is triggered. NULL if the event 
void NsListWatcherAction::onChildChanged(zhandle_t*,const char* path){
	if (path){
		LOG(INFO)<<"NsListWatcherAction children changed under path:"<<path;
		LOG(INFO)<<"reGetNsIdList()";
	}
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->GetNsIdList(rp);
}

//  param path znode path for which the watcher is triggered. NULL if the event 
void GetNsIdIntWatcherAction::onNodeValueChanged(zhandle_t*, const char* path){
	if (path){
		LOG(INFO)<<"GetNsIdIntWatcherAction node value changed of path:"<<path;
		LOG(INFO)<<"reGetNsIdInt()";
	}
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->GetNsIdInt(rp, ns_);			// this path should be modify
}

//  param path znode path for which the watcher is triggered. NULL if the event 
void BucketListWatcherAction::onChildChanged(zhandle_t*,const char* path){
	if (path){
		LOG(INFO)<<"BucketListWatcherAction children changed under path:"<<path;
		LOG(INFO)<<"reGetBucketIDList()";
	}
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->GetBucketIDList(rp);
}

//  param path znode path for which the watcher is triggered. NULL if the event 
void FarmIdNodeWatcherAction::onNodeValueChanged(zhandle_t*, const char* path){
	if (path){
		LOG(INFO)<<"FarmIdNodeWatcherAction node value changed of path:"<<path;
		LOG(INFO)<<"reGetFarmIdOfBucketID()";
	}
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->GetFarmIdOfBucketID(rp, bucketID_);			// this path should be modify
}

//  param path znode path for which the watcher is triggered. NULL if the event 
void FarmListWatcherAction::onChildChanged(zhandle_t*,const char* path){
	if (path){
		LOG(INFO)<<"FarmListWatcherAction children changed under path:"<<path;
		LOG(INFO)<<"reGetFarmList()";
	}
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->GetFarmList(rp);
}

//  param path znode path for which the watcher is triggered. NULL if the event 
void ProxyRolesListWatcherAction::onChildChanged(zhandle_t*,const char* path){
	if (path){
		LOG(INFO)<<"RolesListWatcherAction children changed under path:"<<path;
		LOG(INFO)<<"reGetRolesChildrenList()";
	}
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->GetRolesChildrenList(rp, farm_id_);
}

//  param path znode path for which the watcher is triggered. NULL if the event 
void ProxyVersionNodeWatcherAction::onNodeValueChanged(zhandle_t*, const char* path){
	if (path){
		LOG(INFO)<<"VersionNodeWatcherAction node value changed of path:"<<path;
		LOG(INFO)<<"reGetRolesDesc()";
	}

	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->GetRolesDesc(rp, farm_id_, version_path_);			// this path should be modify
}


}//namespace courier
}//namespace ldd
















