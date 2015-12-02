#ifndef  __LDD_COURIER_ZK_CONTEXT_H_
#define  __LDD_COURIER_ZK_CONTEXT_H_

#include <string>
#include <zookeeper/zookeeper.h>

namespace ldd{
namespace courier{

enum ZkOperationType{
	kAcreateOpt				=1,
	kAdeleteOpt				=2,	
	kAwgetOpt				=3,
	kAwgetChildrenOpt		=4,
	kAwexistsOpt			=5
};

class CompletionCallBack;
class WatcherAction;
class ZkContext{
public:
	
	ZkContext() : milliseconds_(0), zhandle_(NULL), version_(0), create_flags_(0), comp_ctx_(NULL),
		watcher_fn_(NULL), watcherCtx_(NULL){
	}
    // base info
	int64_t				milliseconds_;
	zhandle_t*			zhandle_;
	std::string			path_;

	int					version_;				// zoo_adelete() needed
	std::string			value_;					// zoo_acreate()
	int					create_flags_;			// zoo_acreate()		ZOO_EPHEMERAL | ZOO_SEQUENCE

	ZkOperationType		opt_type_;	
	
	union{
		void_completion_t		void_completion_fun_;		// type define in zookeeper.h
		string_completion_t		string_completion_fun_;
		data_completion_t		data_completion_fun_;
		strings_completion_t	strings_completion_fun;
		stat_completion_t		stat_completion_t_fun;
	}comp_cb;
	CompletionCallBack*			comp_ctx_;
	
	// watcher
	watcher_fn				watcher_fn_;
	WatcherAction*			watcherCtx_;
};

}//namespace courier
}//namespace ldd



#endif











