
#include <sys/time.h>
#include <glog/logging.h>
#include "reactor.h"
#include "zkcontext.h"
#include "completion_callback.h"
#include "watcher_action.h"
#include "redo_policy.h"


namespace ldd{
namespace courier{


ZooKeeperReactor::ZooKeeperReactor(HandleZkError   handle_error, 
                                   const std::string& host_port, int timeout): 
    handle_error_(handle_error),
    host_port_(host_port), 
    started_zkthread_(false), 
	timeout_(timeout), 
    zk_cur_state_(kZkNull),	
    time_event_list_(comparator_){

	zhandle_ = zookeeper_init(host_port_.data(), 
                    ZooKeeperReactor::ZkGlobalWather, timeout, 0, this,0);		
	if (!zhandle_){
		LOG(ERROR)<<"ZooKeeperReactor::zookeeper_init ERROR";
		SetZkState(kZkStartError);
	}	
}

ZooKeeperReactor::~ZooKeeperReactor(){
	DLOG(INFO)<<"~ZooKeeperReactor()";
	
	{
        util::MutexLock l(&mu_);
		while(!time_event_list_.Empty()){
			const ZkContext* te = time_event_list_.Front();
			time_event_list_.PopFront();
			if (te->comp_ctx_){
				RedoPolicy* policy = te->comp_ctx_->GetRedoPolicy();
				delete policy;
				delete te->comp_ctx_;
			}
			if (te->watcherCtx_){
				delete te->watcherCtx_;
			}	
			delete te;
			DLOG(INFO)<<"~ZooKeeperReactor(), clear time_event_list.PopFront";
		}
	}
}

void ZooKeeperReactor::Close(){
	started_zkthread_ = false;
	CloseThread();
	zhandle_ = NULL;
}

void ZooKeeperReactor::CloseThread(){
	DLOG(INFO)<<"before exit close Thread";
	usleep(0);
	pthread_join(zkthread_, NULL);
	DLOG(INFO)<<"clostThread";
}

int ZooKeeperReactor::KeyComparator::operator ()(const ZkContext* a, 
												 const ZkContext* b) const{
	if (a->milliseconds_ <= b->milliseconds_){
		return -1;
	}else if(a->milliseconds_ > b->milliseconds_){
		return 1;
	}else{
		return 0;
	}
}

static const char* state2String(int state){
    if(state==0){
        return "ZOO_CLOSED_STATE";
    }else if(ZOO_CONNECTING_STATE==state){
        return "ZOO_CONNECTING_STATE";
    }else if(state == ZOO_ASSOCIATING_STATE){
        return "ZOO_ASSOCIATING_STATE";
    }else if(state == ZOO_CONNECTED_STATE){
        return "ZOO_CONNECTED_STATE";
    }else if(state == ZOO_EXPIRED_SESSION_STATE){
        return "ZOO_EXPIRED_SESSION_STATE";
    }else if(state == ZOO_AUTH_FAILED_STATE){
        return "ZOO_AUTH_FAILED_STATE";      
    }
    return "INVALID_STATE";
}

void ZooKeeperReactor::ZkGlobalWather(zhandle_t *zh, int type, int state, 
						   const char *path,void *watcher_ctx){
	if (!zh){
        LOG(ERROR)<<"ZooKeeperReactor::ZkGlobalWather() NULL zhandle_t";
		return;
	}
	ZooKeeperReactor* reactor = static_cast<ZooKeeperReactor*>(watcher_ctx);
	
	// connecting_loss, need to delete session_timeout by client
	if (state == ZOO_CONNECTING_STATE && type == ZOO_SESSION_EVENT)  {
		//  first record the time 
		reactor->cur_tick_count_ = reactor->GetTickCount();
		reactor->SetZkState(kZkConnecting);
		LOG(WARNING)<<"ZkGlobalWather() zk try to connecting zkserver timeout="<<reactor->timeout_;
	}
	else if (state == ZOO_CONNECTED_STATE && type == ZOO_SESSION_EVENT)  { // 已经和zk建立连接
		reactor->timeout_ = zoo_recv_timeout(reactor->zhandle_);
		reactor->SetZkState(kZkConnected);
		LOG(INFO)<<"ZkGlobalWather(): zk connect zkserver successfully, timeout="<<reactor->timeout_;
	}else{
		LOG(WARNING)<<"ZkGlobalWather() zk watcher type="<<type<<" state="<<state
            <<" state2String="<<state2String(state);
	}

    /* zookeeper state constants 
        #define EXPIRED_SESSION_STATE_DEF -112
        #define AUTH_FAILED_STATE_DEF -113
        #define CONNECTING_STATE_DEF 1
        #define ASSOCIATING_STATE_DEF 2
        #define CONNECTED_STATE_DEF 3
        
        zookeeper event type constants 
        #define CREATED_EVENT_DEF 1
        #define DELETED_EVENT_DEF 2
        #define CHANGED_EVENT_DEF 3
        #define CHILD_EVENT_DEF 4
        #define SESSION_EVENT_DEF -1
        #define NOTWATCHING_EVENT_DEF -2
    */
}

void ZooKeeperReactor::SetZkState(ZooKeeperState e ){
    zk_cur_state_ = e;

    if (e ==kZkStartError ){
        LOG(ERROR)<<"ZooKeeperReactor::SetZkState() handle_error_ kZkStartError";
        if (handle_error_ != NULL){
            handle_error_(kZkStartError);
        }
        
    }else if (e ==kZkUnRecoveral){
        LOG(ERROR)<<"ZooKeeperReactor::SetZkState() handle_error_ kZkUnRecoveral";
        if (handle_error_ != NULL){
            handle_error_(kZkUnRecoveral);
        }
    }else if (e ==kZkTimeOut){
        LOG(ERROR)<<"ZooKeeperReactor::SetZkState() handle_error_ kZkTimeOut";
        if (handle_error_ != NULL){
            handle_error_(kZkTimeOut);
        }
    }
}
 
ZooKeeperReactor::ZooKeeperState ZooKeeperReactor::GetZkState(){
    return zk_cur_state_;
}

int64_t ZooKeeperReactor::ClientId(){
	const clientid_t *cid = zoo_client_id(zhandle_);
	return cid->client_id;
}

bool  ZooKeeperReactor::StartUp(){	
	if(!started_zkthread_.Load()) {
		started_zkthread_ = true;
		int ret = 0;
		ret = pthread_create(&zkthread_, NULL, &ZooKeeperReactor::ThreadFunc, this);
		if (ret != 0 ){
			started_zkthread_ = false;
			LOG(ERROR)<<"StartUp() pthread_create ERROR, ret="<<ret;
			return false;
		}
		return true;
	}else{
		LOG(ERROR)<<"zookeeper start ERROR";
		return false;
	}
}

void* ZooKeeperReactor::ThreadFunc(void* arg){
	reinterpret_cast<ZooKeeperReactor*>(arg)->run();
	return NULL;
}

/*
	thread body
*/
void ZooKeeperReactor::run(){	
	// should create a request into the queue.
	DLOG(INFO)<<"run() enter zk thread body";
	struct pollfd poll_fd;
	while(started_zkthread_.Load()){
		// get data from queue of ZooKeeperBroker
		{	
            util::MutexLock l(&mu_);
			if(!time_event_list_.Empty()){
				const ZkContext* te = time_event_list_.Front();
				// check should fire this event
				if (ProcessTimeEvent(te)){
					time_event_list_.PopFront();
					delete te;
				}
			}
		}
		
		if ( GetZkState() == kZkConnecting){
			LOG(WARNING)<<"run() zk state is connecting, state="<<GetZkState();
			if (CheckTimeOut()){
				LOG(ERROR)<<"Zk Client time out, ERROR timeout_:"<<timeout_;
				break;
			}
		}
		
		int fd;
		struct timeval tv;
		int interest;
		int timeout;
		int maxfd=1;
		int rc;
		zookeeper_interest(zhandle_,&fd, &interest, &tv);
		if(fd != -1){
			poll_fd.fd = fd;
			poll_fd.events=(interest&ZOOKEEPER_READ)?POLLIN:0;
			poll_fd.events|= (interest&ZOOKEEPER_WRITE)?POLLOUT:0;
		}
		timeout = tv.tv_sec * 1000 + (tv.tv_usec/1000);
		poll(&poll_fd, maxfd, timeout);
		if(fd != -1){
			interest = (poll_fd.revents&POLLIN)?ZOOKEEPER_READ:0;
			interest |= ((poll_fd.revents&POLLOUT)||(poll_fd.revents&POLLHUP))?ZOOKEEPER_WRITE:0;
		}
		rc = zookeeper_process(zhandle_, interest);

		if(zhandle_ && is_unrecoverable(zhandle_)){
            LOG(ERROR)<<"run() Zk Client unrecovered ERROR";
			SetZkState(kZkUnRecoveral); 
			break;
		}
	} // end while
	zookeeper_close(zhandle_);
	LOG(INFO)<<"run() zk thread exit!!! started_zkthread_="<<started_zkthread_.Load();
    if (kZkUnRecoveral  == GetZkState()){   // error handle 
       started_zkthread_ = false;
       LOG(ERROR)<<"run() zk thread exit!!! started_zkthread_="<<started_zkthread_.Load();
    }
    
    
	//log exception
}

int64_t  ZooKeeperReactor::GetTickCount(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec*1000 + tv.tv_usec/1000;
}

bool ZooKeeperReactor::CheckTimeOut(){
	int check_interval_mills		= timeout_/3;
	bool session_timeout			= false;

	if ((GetTickCount() - cur_tick_count_) > check_interval_mills ){
		session_timeout = true;
	}

	if (session_timeout){
		SetZkState(kZkTimeOut);
        LOG(ERROR)<<"ZooKeeperReactor::CheckTimeOut() kZkTimeOut";
	}	
	return session_timeout;
}

bool  ZooKeeperReactor::CreateNode(std::string& node_path, std::string& value, int create_flags, 
								  CompletionCallBack* comp_call_back, WatcherAction* action,
								  int32_t fire_time){
	if (GetZkState() < 0 ){
		LOG(ERROR)<<"zk CreateNode() ERROR; clear event zkState= "<<static_cast<int32_t>(GetZkState());
		delete action;
		if (comp_call_back){
			RedoPolicy* policy = comp_call_back->GetRedoPolicy();
			delete policy;
			delete comp_call_back;
		}
		return false;
	}

	ZkContext *context			= new ZkContext();	// will be delete int CompletionCallBack
	context->milliseconds_		= GetTickCount() + fire_time;
	context->zhandle_			= zhandle_;
	context->path_				= node_path;
	context->value_				= value;
	context->create_flags_		= create_flags;
	context->opt_type_			= kAcreateOpt;

	context->comp_cb.string_completion_fun_ 
								= CompletionCallBack::ZkCreateCompletion;
	context->comp_ctx_			= comp_call_back;			// to set a new 

	context->watcher_fn_		= WatcherAction::WatcherCB;
	context->watcherCtx_		= action;  // may be a wather object(broker_)

	{
        util::MutexLock l(&mu_);
		time_event_list_.Insert(context);
	}
	DLOG(INFO)<<"CreateNode():-"<<" node_path:"<<node_path<<" value:"<<value<<" flags:"<<create_flags<<"  fire_time: "<<fire_time;
	return true;
}

bool  ZooKeeperReactor::Exists(std::string& node_path, CompletionCallBack* comp_call_back,
							   WatcherAction* action, int32_t fire_time){
	if (GetZkState() < 0 ){
	   LOG(ERROR)<<"zk Exists() ERROR; clear event zkState= "<<static_cast<int32_t>(GetZkState());
	   delete action;
	   if (comp_call_back){
		   RedoPolicy* policy = comp_call_back->GetRedoPolicy();
		   delete policy;
		   delete comp_call_back;
	   }
	   return false;
	}
	ZkContext *context			= new ZkContext();	// will be delete int CompletionCallBack
	context->milliseconds_			= GetTickCount() + fire_time;
	context->zhandle_				= zhandle_;
	context->path_				= node_path;
	context->opt_type_			= kAwexistsOpt;

	context->comp_cb.stat_completion_t_fun 
								= CompletionCallBack::ZkExistsCompletion;
	context->comp_ctx_				= comp_call_back;			// to set a new 

	context->watcher_fn_			= WatcherAction::WatcherCB;
	context->watcherCtx_			= action;					// may be a wather object(broker_)

	{
        util::MutexLock l(&mu_);
        time_event_list_.Insert(context);
	}
	DLOG(INFO)<<"Exists() "<<" node_path:"<<node_path<<"  fire_time: "<<fire_time;
	return true;
}

// version=-1 don't check version
bool ZooKeeperReactor::DeleteNode(std::string& node_path, int version, CompletionCallBack* comp_call_back, 
								 WatcherAction* action, int32_t fire_time){
	if (GetZkState() < 0 ){
		LOG(ERROR)<<"zk DeleteNode() ERROR; clear event zkState= "<<static_cast<int32_t>(GetZkState());
		delete action;
		if (comp_call_back){
			RedoPolicy* policy = comp_call_back->GetRedoPolicy();
			delete policy;
			delete comp_call_back;
		}
		return false;
	}
	ZkContext *context			= new ZkContext();	// will be delete int CompletionCallBack
	context->milliseconds_		= GetTickCount() + fire_time;
	context->zhandle_			= zhandle_;
	context->path_				= node_path;
	context->version_			= version;

	context->opt_type_			= kAdeleteOpt;

	context->comp_cb.void_completion_fun_ 
								= CompletionCallBack::ZkDeleteCompletion;
	context->comp_ctx_				= comp_call_back;			// to set a new 

	context->watcher_fn_		= WatcherAction::WatcherCB;
	context->watcherCtx_		= action;  // may be a wather object(broker_)

	{
        util::MutexLock l(&mu_);
		time_event_list_.Insert(context);
	}
	DLOG(INFO)<<"DeleteNode() "<<" node_path:"<<node_path<<" version"<<version<<" fire_time: "<<fire_time;
	return true;
}

bool  ZooKeeperReactor::GetNodeValue(std::string& node_path, CompletionCallBack* comp_call_back, 
								   WatcherAction* action, int32_t fire_time){
	if (GetZkState() < 0 ){
		LOG(ERROR)<<"zk GetNodeValue() ERROR; clear event zkState= "<<static_cast<int32_t>(GetZkState());
		delete action;
		if (comp_call_back){
			RedoPolicy* policy = comp_call_back->GetRedoPolicy();
			delete policy;
			delete comp_call_back;
		}
		return false;
	}
	ZkContext *context			= new ZkContext();	// will be delete int CompletionCallBack
	context->milliseconds_		= GetTickCount() + fire_time;
	context->zhandle_			= zhandle_;
	context->path_				= node_path;
	context->opt_type_			= kAwgetOpt;

	context->comp_cb.data_completion_fun_ 
								= CompletionCallBack::ZkGetCompletion;
	context->comp_ctx_				= comp_call_back;			// to set a new 

	context->watcher_fn_		= WatcherAction::WatcherCB;
	context->watcherCtx_		= action;  // may be a wather object(broker_)

	{
        util::MutexLock l(&mu_);
		time_event_list_.Insert(context);
	}
	DLOG(INFO)<<"GetNodeData() -"<<" path:"<<node_path<<"  fire_time: "<<fire_time;
	return true;
}

// function about context
bool ZooKeeperReactor::GetChildrenList(std::string& node_path, CompletionCallBack* comp_call_back,
									   WatcherAction* action, int32_t fire_time){
	if (GetZkState() < 0 ){
	   LOG(ERROR)<<"zk GetChildrenList() ERROR; clear event zkState= "<<static_cast<int32_t>(GetZkState());
	   delete action;
	   if (comp_call_back){
		   RedoPolicy* policy = comp_call_back->GetRedoPolicy();
		   delete policy;
		   delete comp_call_back;
	   }
	   return false;
	}
	ZkContext *context			= new ZkContext();	// will be delete int CompletionCallBack
	context->milliseconds_		= GetTickCount() + fire_time;
	context->zhandle_			= zhandle_;
	context->path_				= node_path;
	context->opt_type_			= kAwgetChildrenOpt;

	context->comp_cb.strings_completion_fun 
	   = CompletionCallBack::ZkGetChildrenCompletion;
	context->comp_ctx_				= comp_call_back;  //comp_call_back

	context->watcher_fn_		= WatcherAction::WatcherCB;
	context->watcherCtx_		= action;

	{
        util::MutexLock l(&mu_);
        time_event_list_.Insert(context);
	}
	DLOG(INFO)<<"GetChildrenList() -"<<" node_path:"<<node_path<<"  fire_time: "<<fire_time;
	return true;
}


bool ZooKeeperReactor::ProcessTimeEvent(const ZkContext* c){
	bool process = false;

	// check time
	int64_t milliseconds=0;
	milliseconds	= GetTickCount();
	//DLOG(INFO)<<"c->milliseconds_:"<<c->milliseconds_<<" milliseconds:"<<milliseconds;

	if (c->milliseconds_ <= milliseconds){
		process = true;
	}
	
	if (!process){
		return process;
	}

	switch (c->opt_type_){
		case kAcreateOpt:{
			/*zoo_acreate(zhandle_t *zh, const char* path, const char * value, int valuelen, 
			const struct ACL_vector *acl, int flags, 
			string_completion_t completion, const void * data);*/
			zoo_acreate(c->zhandle_, c->path_.data(), c->value_.data(), c->value_.length(),
				&ZOO_OPEN_ACL_UNSAFE, c->create_flags_, c->comp_cb.string_completion_fun_, c->comp_ctx_);

			break;
			}
		case kAdeleteOpt:{
				/*zoo_adelete(zhandle_t * zh, const char* path, int version, 
				void_completion_t completion, const void *data);*/
				zoo_adelete(c->zhandle_, c->path_.data(), c->version_, 
					c->comp_cb.void_completion_fun_, c->comp_ctx_);
				break;
			}
		case kAwgetOpt:{
				/*int zoo_awget(zhandle_t *zh, const char *path, 
				watcher_fn watcher, void* watcherCtx, 
				data_completion_t completion, const void *data);
				*/
				zoo_awget(c->zhandle_, c->path_.data(), c->watcher_fn_, c->watcherCtx_,
					c->comp_cb.data_completion_fun_, c->comp_ctx_);
				break;
			}
		case kAwgetChildrenOpt:{
				/*
				int zoo_awget_children(zhandle_t *zh, const char *path,
				watcher_fn watcher, void* watcherCtx, 
				strings_completion_t completion, const void *data);
				*/
				zoo_awget_children(c->zhandle_, c->path_.data(), c->watcher_fn_, c->watcherCtx_,
					c->comp_cb.strings_completion_fun, c->comp_ctx_);
				break;
			}
		case kAwexistsOpt:{
				/*
				ZOOAPI int zoo_awexists(zhandle_t *zh, const char *path, 
				watcher_fn watcher, void* watcherCtx, 
				stat_completion_t completion, const void *data);
				*/
				zoo_awexists(c->zhandle_, c->path_.data(), c->watcher_fn_, c->watcherCtx_,
					c->comp_cb.stat_completion_t_fun, c->comp_ctx_);
			 }
		default:
			break;
	}	
	DLOG(INFO)<<"ProcessTimeEvent()"<<" type: "<<c->opt_type_<<" path:"<<c->path_;

	return process;
}


}//namespace courier
}//namespace ldd




	
	
