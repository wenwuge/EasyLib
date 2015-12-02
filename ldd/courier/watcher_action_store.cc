
#include "watcher_action_store.h"
#include "zkbroker.h"
#include "zkbroker_store.h"
#include "redo_policy.h"
#include <glog/logging.h>


namespace ldd{
namespace courier{

/************************************************************************/
/* for farm                                                             */
/************************************************************************/
void PrecedorExistWA::onNodeDeleted(zhandle_t*, const char* path){
	LOG(INFO)<<"PrecedorExistWA node:"<<path<<" deleted";
	broker_->DoLeaderElection();
	LOG(INFO)<<"ReStartLeaderElection";
}

void NodeListWatcherAction::onChildChanged(zhandle_t*,const char* path){
	if (path){
		LOG(INFO)<<"NodeListWatcherAction children changed under path:"<<path;
		LOG(INFO)<<"reGetNodeList()";
	}
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->GetNodeList(rp);  
}

//  param path znode path for which the watcher is triggered. NULL if the event 
void RolesListWatcherAction::onChildChanged(zhandle_t*,const char* path){
	if (path){
		LOG(INFO)<<"RolesListWatcherAction children changed under path:"<<path;
		LOG(INFO)<<"reGetRolesChildrenList()";
	}
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->GetRolesChildrenList(rp);
}

//  param path znode path for which the watcher is triggered. NULL if the event 
void VersionNodeWatcherAction::onNodeValueChanged(zhandle_t*, const char* path){
	if (path){
		LOG(INFO)<<"VersionNodeWatcherAction node value changed of path:"<<path;
		LOG(INFO)<<"reGetRolesDesc()";
	}
	
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->GetRolesDesc(rp, version_path_);			// this path should be modify
}

//  param path znode path for which the watcher is triggered. NULL if the event 
void AliveNodeListWatcherAction::onChildChanged(zhandle_t*,const char* path){
	if (path){
		LOG(INFO)<<"AliveNodeListWatcherAction children changed under path:"<<path;
		LOG(INFO)<<"reGetAliveNodeList()";
	}
	RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
	broker_->GetAliveNodeList(rp);
}


}//namespace courier
}//namespace ldd
















