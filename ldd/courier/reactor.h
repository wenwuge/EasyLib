

#ifndef __LDD_COURIER_ZK_REACTOR_H_
#define __LDD_COURIER_ZK_REACTOR_H_

#include <pthread.h> 
#include <string>
#include <poll.h>
#include <stdint.h>
#include <sys/time.h>
#include <zookeeper/zookeeper.h>
#include <ldd/util/atomic.h>
#include <ldd/util/mutex.h>

#include "EventList.h"
#include "options.h"




namespace ldd{
namespace courier{

class Context;
class ZkContext;
class CompletionCallBack;
class WatcherAction;

class ZooKeeperReactor
{
public:
	ZooKeeperReactor(HandleZkError  handle_error, 
                     const std::string& host_port, int timeout);
	~ZooKeeperReactor();

public:
	bool StartUp();

public:
	//	 zk interface 
	bool  CreateNode(std::string& node_path, std::string& value, int create_flags, 
					CompletionCallBack* comp_call_back, WatcherAction* action, 
					int32_t fire_time);
	bool  Exists(std::string& node_path, CompletionCallBack* comp_call_back, 
					WatcherAction* action, int32_t fire_time);
	bool  GetNodeValue(std::string& node_path,CompletionCallBack* comp_call_back,
					WatcherAction* action, int32_t fire_time);
	bool  DeleteNode(std::string& node_path, int version, CompletionCallBack* comp_call_back, 
						WatcherAction* action, int32_t fire_time);
	bool  GetChildrenList(std::string& node_path, CompletionCallBack* comp_call_back, 
						 WatcherAction* action, int32_t fire_time);
    
	enum ZooKeeperState{ 
		// not ok
		kZkStartError	= -1,
		kZkUnRecoveral  = -2,
		kZkTimeOut		= -3,

		// ok
		kZkNull			= 0,		// before connecting, after zookeeper_init()
		kZkConnecting	= 1,
		kZkConnected	= 2,
		
	};

	int64_t ClientId();
	void	Close();

private:
	static void*	ThreadFunc(void* arg);
	void			CloseThread();
	void			run();
	bool			ProcessTimeEvent(const ZkContext* c);
	static void		ZkGlobalWather(zhandle_t *zh, int type, int state, 
								const char *path,void *watcherCtx);
	bool			CheckTimeOut();
	int64_t			GetTickCount();
	void			SetZkState(ZooKeeperState e );
	ZooKeeperState	GetZkState();

    HandleZkError   handle_error_;
	const std::string	host_port_;
	zhandle_t*			zhandle_;
    util::Atomic<bool>			started_zkthread_;
	int					timeout_;
	pthread_t			zkthread_;
    util::Atomic<ZooKeeperState> zk_cur_state_;
	int64_t				cur_tick_count_;
private:
	// about the context
	class KeyComparator{
	public:
		KeyComparator(){}
		int operator()(const ZkContext* a, const ZkContext* b) const;
	};
	KeyComparator comparator_;
	typedef EventList<const ZkContext*, KeyComparator> EventListEx;
	EventListEx time_event_list_;
    util::Mutex mu_;
};


}//namespace courier
}//namespace ldd

#endif // __LDD_COURIER_ZK_REACTOR_H_



