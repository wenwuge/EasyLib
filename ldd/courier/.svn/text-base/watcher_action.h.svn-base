#ifndef WATCHER__ACTION_H_
#define WATCHER__ACTION_H_

#include <zookeeper/zookeeper.h>



namespace ldd{
namespace courier{


class WatcherAction{
public:
	WatcherAction();
	virtual ~WatcherAction();

	static void	WatcherCB(zhandle_t *zh, int type, int state, const char *path,void* ctx);

	virtual void onNodeCreated(zhandle_t *, const char* path){}
	virtual void onSessionExpired(zhandle_t*){}
	virtual void onConnectionEstablished(zhandle_t*){}
	virtual void onConnectionLost(zhandle_t*){}
	virtual void onNodeValueChanged(zhandle_t*,const char* path){}
	virtual void onNodeDeleted(zhandle_t*,const char* path){}
	virtual void onChildChanged(zhandle_t*,const char* path){}

private:
	WatcherAction(const WatcherAction&);
	void operator=(const WatcherAction&);
};



}//namespace courier
}//namespace ldd




#endif
















