
#ifndef __LDD_COURIER_COMPLETION_CALL_BACK_H_
#define __LDD_COURIER_COMPLETION_CALL_BACK_H_

#include <zookeeper/zookeeper.h>

namespace ldd{
namespace courier{

class RedoPolicy;
class CompletionCallBack{
public:
	CompletionCallBack();
	virtual ~CompletionCallBack();

	/**
	== string_completion_t()
	only call back when client call    zoo_acreate(), (zoo_async() not support )
	*/
	static void ZkCreateCompletion(int rc, const char* value, const void*data);

	//typedef void (*stat_completion_t)(int rc, const struct Stat *stat,
	//	const void *data);
	
	static void ZkExistsCompletion(int rc, const struct Stat *stat,
									const void *data);

	/**
		// == void_completion_t()
		only call back when client call    zoo_adelete(), (zoo_aset_acl(), zoo_add_auth()  not support )
	*/
	static void ZkDeleteCompletion(int rc, const void *data);

	/**
	   ==  data_completion_t()
		only call back when client call    zoo_awget(), (zoo_aget() not support )
	*/
	static void ZkGetCompletion(int rc, const char * value, int value_len, 
								const struct Stat* stat, const void * data);

	/**
			== strings_completion_t()
		only call back when client call    zoo_awget_children(), (zoo_aget_children() not support )
		String_vector define in /generated/zookeeper.jute.h
	*/    
	static void ZkGetChildrenCompletion(int rc, const struct String_vector *strings, 
									const void* data);

	static void NotOkHandle(CompletionCallBack * cb, int rc);


	virtual void CreateCompletion(const char* value) {}
	virtual void ExistsCompletion(const struct Stat *stat){}
	virtual void DeleteCompletion(){}
	virtual void GetCompletion(const char * value, int value_len, 
								const struct Stat* stat){}
	virtual void GetChildrenCompletion(const struct String_vector *strings){}

	virtual void Retry(RedoPolicy* redo_policy_){};
	virtual void ErrorHandle(){};
public:
	void SetRedoPolicy(RedoPolicy* redo_policy);
	RedoPolicy* GetRedoPolicy();

private:
	CompletionCallBack(const CompletionCallBack&);
	void operator=(const CompletionCallBack&);
private:
	RedoPolicy* redo_policy_c_;
};



}//namespace courier
}//namespace ldd




#endif













