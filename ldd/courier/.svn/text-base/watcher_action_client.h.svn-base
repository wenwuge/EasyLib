
#ifndef __LDD_COURIER_WATCHER_ACTION_CLIENT_H_
#define __LDD_COURIER_WATCHER_ACTION_CLIENT_H_
#include "watcher_action.h"
#include "zkbroker_client.h"
#include "redo_policy.h"



namespace ldd{
namespace courier{


class ClientNodeListWatcherAction : public WatcherAction{
public:
	ClientNodeListWatcherAction(ZkBrokerClient* b ): broker_(b){};
	virtual ~ClientNodeListWatcherAction(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onChildChanged(zhandle_t*,const char* path){
		if (path){
			LOG(INFO)<<"PnodeListWatcherAction children changed under path:"<<path;
			LOG(INFO)<<"reGetNodeList()";
		}
		RedoPolicy* rp = broker_->CurRedoPolicy()->Copy();
		broker_->GetProxyList(rp); 
	}
private:
	ClientNodeListWatcherAction(const ClientNodeListWatcherAction&);
	void operator=(const ClientNodeListWatcherAction&);

private:
	ZkBrokerClient* broker_;
};


}//namespace courier
}//namespace ldd




#endif
















