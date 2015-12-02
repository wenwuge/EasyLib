#ifndef __LDD_CLIENT_SYN_CLIENT_COMPLETION__H_
#define __LDD_CLIENT_SYN_CLIENT_COMPLETION__H_

#include <glog/logging.h>
#include "ldd/util/mutex.h"
#include "countdownlatch.h"

using namespace ldd::util;


namespace ldd {
namespace client {
namespace syn {

class SynCompletion{
public:
	SynCompletion() : version_(-1),ttl_(0), cond_(&mutex_), 
				completed_(false), latch_(NULL){		

	}

public:
	void DataCompletion(int64_t version, const std::string& value, uint32_t ttl){
		version_ = version;
		value_	= value;
		ttl_ = ttl;
		LOG(INFO)<<"DataCompletion() Signal()";
		if (latch_){
			latch_->CountDown();
		}
		Signal();
	}
	void CheckCompletion(int64_t version, uint32_t ttl){
		version_ = version;
		ttl_ = ttl;
		LOG(INFO)<<"CheckCompletion() Signal()";
		if (latch_){
			latch_->CountDown();
		}
		Signal();
	}

	void MutateCompletion(int64_t version){
		version_ = version;
		LOG(INFO)<<"MutateCompletion() Signal()";
		if (latch_){
			latch_->CountDown();
		}
		Signal();
	};

	void CasCompletion(int64_t version, const std::string& value){
		version_ = version;
		value_   = value;
		LOG(INFO)<<"CasCompletion() Signal()";
		if (latch_){
			latch_->CountDown();
		}
		Signal();
	}

	void WaitCompletion(){
		MutexLock lock(&mutex_);
		while(!completed_){
			LOG(INFO)<<"wait()";
			cond_.Wait();
		}
	}

	std::string& key(){
		return key_;
	}

	int64_t version(){
		return version_;
	}

	uint32_t ttl(){
		return ttl_;
	}

	std::string& value(){
		return value_;
	}

	void SetInfo(std::string key, CountDownLatch* latch){
		key_= key;
		latch_ = latch;
	}

private:
	void Signal(){
		MutexLock lock(&mutex_);
		completed_ = true;
		cond_.Signal();
	}

private:
	int64_t			version_;
	uint32_t		ttl_;
	std::string		key_;
	std::string		value_;

	Mutex   mutex_;
	CondVar cond_;
	bool		       completed_;
	
	CountDownLatch* latch_;
};

} // namespace ldd {
} // namespace client {
} // namespace syn {


#endif



