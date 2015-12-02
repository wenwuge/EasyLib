#ifndef __LDD_COURIER_COMPLETION_CALLBACK_IMPL_H_
#define __LDD_COURIER_COMPLETION_CALLBACK_IMPL_H_

#include "completion_callback.h"



namespace ldd{
namespace courier{

class ZooKeeperBroker;

class ReplicaNodeCB : public CompletionCallBack{
public:
	ReplicaNodeCB(ZooKeeperBroker* b): broker_(b){
	};

	virtual void GetCompletion(const char * value, int value_len, 
								const struct Stat* stat);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZooKeeperBroker* broker_;
};

class EngineNodeCB : public CompletionCallBack{
public:
	EngineNodeCB(ZooKeeperBroker* b): broker_(b){
	};

	virtual void GetCompletion(const char * value, int value_len, 
		const struct Stat* stat);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZooKeeperBroker* broker_;
};

class GetMaxBinLogCB : public CompletionCallBack{
public:
	GetMaxBinLogCB(ZooKeeperBroker* b): broker_(b){
	};

	virtual void GetCompletion(const char * value, int value_len, 
		const struct Stat* stat);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZooKeeperBroker* broker_;
};

class GetBucketSizeCB : public CompletionCallBack{
public:
	GetBucketSizeCB(ZooKeeperBroker* b): broker_(b){
	};

	virtual void GetCompletion(const char * value, int value_len, 
		const struct Stat* stat);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZooKeeperBroker* broker_;
};

class BucketListCB : public CompletionCallBack{
public:
	BucketListCB(ZooKeeperBroker* b): broker_(b){
	};

	virtual void GetChildrenCompletion(const struct String_vector *strings);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZooKeeperBroker* broker_;
};
class GetFarmIdOfBucketIdCB : public CompletionCallBack{
public:
	GetFarmIdOfBucketIdCB(ZooKeeperBroker* b, std::string& bucketID) : 
	  broker_(b), bucketID_(bucketID){};

	  virtual void GetCompletion(const char * value, int value_len, 
		  const struct Stat* stat);
	  virtual void Retry(RedoPolicy* redo_policy_);
	  virtual void ErrorHandle();
private:
	ZooKeeperBroker* broker_;
	std::string	bucketID_;
};

}//namespace courier
}//namespace ldd



#endif








