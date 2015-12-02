
#ifndef __LDD_COURIER_WATCHER_ACTION_STORE_H_
#define __LDD_COURIER_WATCHER_ACTION_STORE_H_
#include <string>
#include "watcher_action.h"



namespace ldd{
namespace courier{

/************************************************************************/
/* for farm watcher                                                     */
/************************************************************************/
class ZkBrokerStore;
class PrecedorExistWA : public WatcherAction{
public:
	PrecedorExistWA(ZkBrokerStore*  b) :broker_(b){}

	virtual ~PrecedorExistWA(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onNodeDeleted(zhandle_t*, const char* path);

private:
	PrecedorExistWA(const PrecedorExistWA&);
	void operator=(const PrecedorExistWA&);

private:
	ZkBrokerStore*  broker_;
};

class NodeListWatcherAction : public WatcherAction{
public:
	NodeListWatcherAction(ZkBrokerStore* b ): broker_(b){};
	virtual ~NodeListWatcherAction(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onChildChanged(zhandle_t*,const char* path);
private:
	NodeListWatcherAction(const NodeListWatcherAction&);
	void operator=(const NodeListWatcherAction&);

private:
	ZkBrokerStore* broker_;
};



class RolesListWatcherAction : public WatcherAction{
public:
	RolesListWatcherAction(ZkBrokerStore* b ): broker_(b){};
	virtual ~RolesListWatcherAction(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onChildChanged(zhandle_t*,const char* path);

private:
	RolesListWatcherAction(const RolesListWatcherAction&);
	void operator=(const RolesListWatcherAction&);

private:
	ZkBrokerStore* broker_;
};

class VersionNodeWatcherAction : public WatcherAction{
public:
	VersionNodeWatcherAction(ZkBrokerStore* b, std::string version_path): 
					broker_(b), version_path_(version_path){};

	virtual ~VersionNodeWatcherAction(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onNodeValueChanged(zhandle_t*, const char* path);

private:
	VersionNodeWatcherAction(const VersionNodeWatcherAction&);
	void operator=(const VersionNodeWatcherAction&);

private:
	ZkBrokerStore* broker_;
	std::string version_path_;
};

class AliveNodeListWatcherAction : public WatcherAction{
public:
	AliveNodeListWatcherAction(ZkBrokerStore* b ): broker_(b){};
	virtual ~AliveNodeListWatcherAction(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onChildChanged(zhandle_t*,const char* path);
private:
	AliveNodeListWatcherAction(const AliveNodeListWatcherAction&);
	void operator=(const AliveNodeListWatcherAction&);

private:
	ZkBrokerStore* broker_;
};


}//namespace courier
}//namespace ldd




#endif
















