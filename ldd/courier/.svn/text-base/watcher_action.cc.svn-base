#include "watcher_action.h"



namespace ldd{
namespace courier{


WatcherAction::WatcherAction(){
}

WatcherAction::~WatcherAction(){
}

void WatcherAction::WatcherCB(zhandle_t *zh, int type, int state, const char *path,void* ctx){
	WatcherAction* action=(WatcherAction*)ctx;
	do 
	{
		if(zh==0 || ctx==0) {
			break;
		}

		if(type==ZOO_SESSION_EVENT){
			// do nothing, this type will be handle by global watcher
			break;
			if(state==ZOO_EXPIRED_SESSION_STATE){
				action->onSessionExpired(zh);
			}
			else if(state==ZOO_CONNECTING_STATE){
				action->onConnectionLost(zh);
			}
			else if(state==ZOO_CONNECTED_STATE){
				action->onConnectionEstablished(zh);
			}
		}else if (type == ZOO_CREATED_EVENT){
			action->onNodeCreated(zh, path);
		}
		else if(type==ZOO_CHANGED_EVENT){
			action->onNodeValueChanged(zh,path);
		}
		else if(type==ZOO_DELETED_EVENT){
			action->onNodeDeleted(zh,path);
		}
		else if(type==ZOO_CHILD_EVENT){
			action->onChildChanged(zh,path);
		}
	} while (0);
	if (type != ZOO_SESSION_EVENT){
		delete action;		// why delete action of this type will core this watcher may not be triggerd, so memory leaked  // todo
	}
}



}//namespace courier
}//namespace ldd



















