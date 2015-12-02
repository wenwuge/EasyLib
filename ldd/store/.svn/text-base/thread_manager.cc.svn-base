

#include "thread_manager.h"
#include "glog/logging.h"

using namespace std;

namespace ldd{
namespace storage{

Runnable::Runnable() { 
}

Runnable::~Runnable(){
     LOG(ERROR)<<"Runnable::~Runnable()";
}

void Runnable::run() {
	LOG(ERROR)<<"in the virtual run function";
	assert(false);
}


void *wrapper(void *param) {
	ThreadManager *t = (ThreadManager *) param;
	t->worker();
	return NULL;
}

ThreadManager::ThreadManager(unsigned int size):
    cond_(&mutex0_), thread_num_(size){
	running_ = true;
	running_jobs_ = 0;
	if (size > 1000) {
		fputs("The thread_num is too big\n", stderr);
		assert(false);
	}

	threads_ = new pthread_t[size];
	for (unsigned int i = 0; i < size; i++) {
		/*int rc =*/ pthread_create(&threads_[i], NULL, wrapper, (void *)this);
		//assert(rc == 0);
	}
	LOG(ERROR)<<"created threads num="<<thread_num_;
}

void ThreadManager::sync(){
	while (running_jobs_ > 0) {
		usleep(10); 
	}
}

unsigned int ThreadManager::size() {
	return running_jobs_;
}

ThreadManager::~ThreadManager(){
	this->sync();
	mutex0_.Lock();
    running_ = false;
    cond_.SignalAll();
    mutex0_.Unlock();
    LOG(ERROR)<<"ThreadManager::~ThreadManager() SignaleAll()";

	for (unsigned int i = 0; i < thread_num_; i++) {
		if (pthread_join(threads_[i], NULL) != 0) {
			LOG(ERROR)<<"joined the  thread error num="<<i;
			assert(false);
		}
	}
	LOG(ERROR)<<"ThreadManager::~ThreadManager() joined all threads, num="<<thread_num_;

	delete[]threads_;
}

void ThreadManager::worker() {
	for (; running_; ) {	

		/*mutex0_.Lock();
		if (!running_.Load()){
			LOG(ERROR)<<"running = false";
			break;
		}
		mutex0_.Unlock();*/

		mutex0_.Lock();
		while(jobs_.empty() == true && running_.Load()){
			LOG(ERROR)<<"ThreadManager::worker(), Waiting";
			cond_.Wait();
		}

        if (!running_.Load()){
            LOG(ERROR)<<"running = false";
            mutex0_.Unlock();
            break;
        }
		
        LOG(ERROR)<<"ThreadManager::worker() unlock size="<<jobs_.size();
		Runnable *ajob = NULL;
		if (jobs_.size() > 0) {
			ajob = jobs_.front();
			jobs_.pop();
		}
		mutex0_.Unlock();

        LOG(ERROR)<<"ThreadManager::worker() unlock size="<<jobs_.size();
		if (ajob) {						//succ acquire a Job
			ajob->run();
			mutex1_.Lock();
			running_jobs_--;
			mutex1_.Unlock();
		} 
	}

    LOG(ERROR)<<"ThreadManager::worker() exit";
}

void ThreadManager::append(Runnable * ajob) {
	mutex0_.Lock();
    LOG(ERROR)<<"ThreadManager::append() get lock";
	while (jobs_.size() > this->thread_num_ * 2) {
		usleep(10);				
	}
	running_jobs_++;
	jobs_.push(ajob);
	//if (empty){
		LOG(ERROR)<<"ThreadManager::append(), signal() jobs_.size()="<<jobs_.size();
		cond_.Signal();
	//}
	
	mutex0_.Unlock();
    LOG(ERROR)<<"ThreadManager::append() exit";
} 

}//namespace storage
}//namespace ldd


