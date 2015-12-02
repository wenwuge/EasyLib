
#ifndef __LDD_COURIER_WATCHER_ACTION_PROXY_H_
#define __LDD_COURIER_WATCHER_ACTION_PROXY_H_
#include <string>
#include "watcher_action.h"



namespace ldd{
namespace courier{


class ZkBrokerProxy;

class PnodeListWatcherAction : public WatcherAction{
public:
	PnodeListWatcherAction(ZkBrokerProxy* b ): broker_(b){};
	virtual ~PnodeListWatcherAction(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onChildChanged(zhandle_t*,const char* path);
private:
	PnodeListWatcherAction(const PnodeListWatcherAction&);
	void operator=(const PnodeListWatcherAction&);

private:
	ZkBrokerProxy* broker_;
};

class NsListWatcherAction : public WatcherAction{
public:
	NsListWatcherAction(ZkBrokerProxy* b ): broker_(b){};
	virtual ~NsListWatcherAction(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onChildChanged(zhandle_t*,const char* path);

private:
	NsListWatcherAction(const NsListWatcherAction&);
	void operator=(const NsListWatcherAction&);

private:
	ZkBrokerProxy* broker_;
};

class GetNsIdIntWatcherAction : public WatcherAction{
public:
	GetNsIdIntWatcherAction(ZkBrokerProxy* b, std::string& ns):
	  broker_(b), ns_(ns){};

	  virtual ~GetNsIdIntWatcherAction(){};
	  //  param path znode path for which the watcher is triggered. NULL if the event 
	  virtual void onNodeValueChanged(zhandle_t*, const char* path);

private:
	GetNsIdIntWatcherAction(const GetNsIdIntWatcherAction&);
	void operator=(const GetNsIdIntWatcherAction&);

private:
	ZkBrokerProxy* broker_;
	std::string ns_;
};

class BucketListWatcherAction : public WatcherAction{
public:
	BucketListWatcherAction(ZkBrokerProxy* b ): broker_(b){};
	virtual ~BucketListWatcherAction(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onChildChanged(zhandle_t*,const char* path);

private:
	BucketListWatcherAction(const BucketListWatcherAction&);
	void operator=(const BucketListWatcherAction&);

private:
	ZkBrokerProxy* broker_;
};

class FarmIdNodeWatcherAction : public WatcherAction{
public:
	FarmIdNodeWatcherAction(ZkBrokerProxy* b, std::string& bucketID):
	  broker_(b), bucketID_(bucketID){};

	  virtual ~FarmIdNodeWatcherAction(){};
	  //  param path znode path for which the watcher is triggered. NULL if the event 
	  virtual void onNodeValueChanged(zhandle_t*, const char* path);

private:
	FarmIdNodeWatcherAction(const FarmIdNodeWatcherAction&);
	void operator=(const FarmIdNodeWatcherAction&);

private:
	ZkBrokerProxy* broker_;
	std::string bucketID_;
};

/************************************************************************/
/* useful                                                               */
/************************************************************************/

class FarmListWatcherAction : public WatcherAction{
public:
	FarmListWatcherAction(ZkBrokerProxy* b ): broker_(b){};

	virtual ~FarmListWatcherAction(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onChildChanged(zhandle_t*,const char* path);

private:
	FarmListWatcherAction(const FarmListWatcherAction&);
	void operator=(const FarmListWatcherAction&);

private:
	ZkBrokerProxy* broker_;
};

class ProxyRolesListWatcherAction : public WatcherAction{
public:
	ProxyRolesListWatcherAction(ZkBrokerProxy* b , const  std::string& farm_id):
			broker_(b),farm_id_(farm_id){};
	virtual ~ProxyRolesListWatcherAction(){};
	//  param path znode path for which the watcher is triggered. NULL if the event 
	virtual void onChildChanged(zhandle_t*,const char* path);

private:
	ProxyRolesListWatcherAction(const ProxyRolesListWatcherAction&);
	void operator=(const ProxyRolesListWatcherAction&);

private:
	ZkBrokerProxy* broker_;
	const std::string farm_id_;
};

class ProxyVersionNodeWatcherAction : public WatcherAction{
public:
	ProxyVersionNodeWatcherAction(ZkBrokerProxy* b, 
		const std::string& farm_id, const std::string& version_path): 
	  broker_(b), farm_id_(farm_id), version_path_(version_path){};

	  virtual ~ProxyVersionNodeWatcherAction(){};
	  //  param path znode path for which the watcher is triggered. NULL if the event 
	  virtual void onNodeValueChanged(zhandle_t*, const char* path);

private:
	ProxyVersionNodeWatcherAction(const ProxyVersionNodeWatcherAction&);
	void operator=(const ProxyVersionNodeWatcherAction&);

private:
	ZkBrokerProxy* broker_;
	const std::string farm_id_;
	const std::string version_path_;
};

}//namespace courier
}//namespace ldd




#endif
















