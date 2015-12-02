
#ifndef __LDD_COURIER_WATCHER_ACTION_IMPL_H_
#define __LDD_COURIER_WATCHER_ACTION_IMPL_H_
#include "watcher_action.h"



namespace ldd{
namespace courier{

class ZooKeeperBroker;

class ReplicaNodeWatcherAction : public WatcherAction{
public:
	ReplicaNodeWatcherAction(ZooKeeperBroker* b ) : broker_(b){}
	virtual ~ReplicaNodeWatcherAction(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onNodeValueChanged(zhandle_t*,const char* path);

private:
	ReplicaNodeWatcherAction(const ReplicaNodeWatcherAction&);
	void operator=(const ReplicaNodeWatcherAction&);

private:
	ZooKeeperBroker* broker_;
};

class StoreEngineNodeWatcherAction : public WatcherAction{
public:
	StoreEngineNodeWatcherAction(ZooKeeperBroker* b ) : broker_(b){}

	virtual ~StoreEngineNodeWatcherAction(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onNodeValueChanged(zhandle_t*,const char* path);

private:
	StoreEngineNodeWatcherAction(const StoreEngineNodeWatcherAction&);
	void operator=(const StoreEngineNodeWatcherAction&);

private:
	ZooKeeperBroker* broker_;
};

class BinLogNodeWatcherAction : public WatcherAction{
public:
	BinLogNodeWatcherAction(ZooKeeperBroker* b ) : broker_(b){}

	virtual ~BinLogNodeWatcherAction(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onNodeValueChanged(zhandle_t*,const char* path);

private:
	BinLogNodeWatcherAction(const BinLogNodeWatcherAction&);
	void operator=(const BinLogNodeWatcherAction&);

private:
	ZooKeeperBroker* broker_;
};

class BucketSizeNodeWatcherAction : public WatcherAction{
public:
	BucketSizeNodeWatcherAction(ZooKeeperBroker* b ) : broker_(b){}

	virtual ~BucketSizeNodeWatcherAction(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onNodeValueChanged(zhandle_t*,const char* path);

private:
	BucketSizeNodeWatcherAction(const BucketSizeNodeWatcherAction&);
	void operator=(const BucketSizeNodeWatcherAction&);

private:
	ZooKeeperBroker* broker_;
};


}//namespace courier
}//namespace ldd




#endif
















