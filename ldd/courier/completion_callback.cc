
#include <glog/logging.h>
#include "completion_callback.h"
#include "zkcontext.h"
#include "redo_policy.h"

namespace ldd{
namespace courier{


CompletionCallBack::CompletionCallBack(): redo_policy_c_(NULL){
}


CompletionCallBack::~CompletionCallBack(){

}

void CompletionCallBack::SetRedoPolicy(RedoPolicy* redo_policy){
	redo_policy_c_ = redo_policy;
}

RedoPolicy* CompletionCallBack::GetRedoPolicy(){
	return redo_policy_c_;
}

/**
== string_completion_t()
only call back when client call    zoo_acreate(), (zoo_async() not support )
*/
void CompletionCallBack::ZkCreateCompletion(int rc, const char* value, 
											const void*data){	
	if (rc != 0){
		LOG(ERROR)<<"ERROR ZkCreateCompletion  rc=" << rc;
	}
	if (!data){
		return;
	}
	CompletionCallBack * cb = static_cast<CompletionCallBack*>(const_cast<void *>(data));

	if (rc == ZOK){
		cb->CreateCompletion(value);
		delete cb->redo_policy_c_;
	}else{
		NotOkHandle(cb,rc);
	}
	
	delete cb;
}

void CompletionCallBack::ZkExistsCompletion(int rc, const struct Stat *stat,
											const void *data){	
	if (rc != 0){
		LOG(ERROR)<<"ERROR ZkExistsCompletion  rc=" << rc;
	}
	if (!data){
		return;
	}


	CompletionCallBack * cb = static_cast<CompletionCallBack*>(const_cast<void *>(data));
	if (rc == ZOK){
		cb->ExistsCompletion(stat);
		delete cb->redo_policy_c_;
	}else{
		NotOkHandle(cb,rc);
	}
	
	delete cb;
}

/**
	// == void_completion_t()
	only call back when client call    zoo_adelete(), (zoo_aset_acl(), zoo_add_auth()  not support )
*/
void CompletionCallBack::ZkDeleteCompletion(int rc, const void *data){	
	if (rc != 0){
		LOG(ERROR)<<"ERROR ZkDeleteCompletion  rc=" << rc;
	}
	if (!data){
		return;
	}

	CompletionCallBack * cb = static_cast<CompletionCallBack*>(const_cast<void *>(data));

	if (rc == ZOK){
		cb->DeleteCompletion();
		delete cb->redo_policy_c_;
	}else{
		NotOkHandle(cb,rc);
	}

	delete cb;
}

/**
   ==  data_completion_t()
	only call back when client call    zoo_awget(), (zoo_aget() not support )
*/
void CompletionCallBack::ZkGetCompletion(int rc, const char * value, 
										 int value_len, 
							const struct Stat* stat, const void * data){
	if (rc != 0){
		LOG(ERROR)<<"ERROR ZkGetCompletion  rc=" << rc;
	}
	if (!data){
		return;
	}
	// data may be context, need delete in this function 
	CompletionCallBack * cb = static_cast<CompletionCallBack*>(const_cast<void *>(data));

	if (rc == ZOK){
		cb->GetCompletion(value, value_len, stat);
		delete cb->redo_policy_c_;
	}else{
		NotOkHandle(cb,rc);
	}

	delete cb;
}

/**
		== strings_completion_t()
	only call back when client call    zoo_awget_children(), (zoo_aget_children() not support )
	String_vector define in /generated/zookeeper.jute.h
*/    
void CompletionCallBack::ZkGetChildrenCompletion(int rc, 
											 const struct String_vector *strings, 
											 const void* data){	
	 if (rc != 0){
		 LOG(ERROR)<<"ERROR ZkGetChildrenCompletion  rc=" << rc;
	 }
	 if (!data){
		 return;
	 }
	 //data may be context, need delete in this function 
	 CompletionCallBack * cb = static_cast<CompletionCallBack*>(const_cast<void *>(data));

	 if (rc == ZOK){
		 cb->GetChildrenCompletion(strings);
		 delete cb->redo_policy_c_;
	 }else{
		 NotOkHandle(cb,rc);
	 }

	 delete cb;
}

void CompletionCallBack::NotOkHandle(CompletionCallBack * cb, int rc){
	if (rc == ZCONNECTIONLOSS){
		LOG(WARNING)<<"ERROR CompletionCallBack::NotOkHandle zookeeper : ZCONNECTIONLOSS ";
		if (cb->redo_policy_c_ == NULL){
			cb->ErrorHandle();
			return ;
		}

		cb->redo_policy_c_->incr();
		if(cb->redo_policy_c_->ShouldRety()){
			LOG(WARNING)<<"CompletionCallBack  ShouldRety=true ZCONNECTIONLOSS£¬ times="
				<<cb->redo_policy_c_->CurTimes();
			cb->Retry(cb->redo_policy_c_);
		}else{
			LOG(WARNING)<<"CompletionCallBack  ShouldRety=false ZCONNECTIONLOSS ";
			cb->ErrorHandle();
			delete cb->redo_policy_c_;
		}
	}else {
		cb->ErrorHandle();
		delete cb->redo_policy_c_;
	}
}


}//namespace courier
}//namespace ldd













