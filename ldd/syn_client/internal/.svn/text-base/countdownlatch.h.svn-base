#ifndef __LDD_CLIENT_SYN_COUNT_DOWN_LATCH_H_
#define __LDD_CLIENT_SYN_COUNT_DOWN_LATCH_H_

#include <glog/logging.h>
#include "ldd/util/mutex.h"

using namespace ldd::util;


namespace ldd {
namespace client {
namespace syn {

class CountDownLatch{
public:
	CountDownLatch(int count) : count_(count),cond_(&mutex_){

	}

public:
	void Wait(){
		MutexLock lock(&mutex_);
		while (count_ > 0){
			cond_.Wait();
		}
	}

	void CountDown(){
		MutexLock lock(&mutex_);
		count_--;
		if (count_ == 0 )
		{
			cond_.SignalAll();
		}
	}

private:
	int count_;	
	Mutex   mutex_;
	CondVar cond_;
 	

};

} // namespace ldd {
} // namespace client {
} // namespace syn {


#endif



