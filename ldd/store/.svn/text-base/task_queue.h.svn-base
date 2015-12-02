
#ifndef __LDD__TAST_QUEUE_H_
#define __LDD__TAST_QUEUE_H_

#include <deque>
#include <glog/logging.h>
#include "ldd/util/mutex.h"
#include "singleton_holder.h"
#include "task.h"


namespace ldd{
namespace storage{

//class Task;
class  BlockQueue{
public:
	BlockQueue() : cond_(&mutex_), destroy_(false),seq_(0){
		
	}
	~BlockQueue(){
		
	}

	void Destory(){
		ldd::util::MutexLock l(&mutex_);
        destroy_ = true;
		cond_.SignalAll();
		DLOG(INFO)<<"BlockQueue::Destory(), SignalAll()";
	}

	void push_back(Task* t){
		ldd::util::MutexLock l(&mutex_);
		if (destroy_== false){
			//bool empty = task_queue_.empty();	
			task_queue_.push_back(t);
            seq_++;
            t->SetSeq(seq_);
			//if (empty){
				cond_.Signal();	
                DLOG(INFO)<<"BlockQueue::push_back() Signal()";
			//}
			DLOG(INFO)<<"BlockQueue::push_back() exit";
		}
	}

    void push_front(Task* t){
        ldd::util::MutexLock l(&mutex_);
        if (destroy_== false){
            //bool empty = task_queue_.empty();	
            task_queue_.push_front(t);
            /*seq_++;
            t->SetSeq(seq_);*/
            //if (empty){
                cond_.Signal();	
                DLOG(INFO)<<"BlockQueue::push_front() Signal!!";
            //}
            DLOG(INFO)<<"BlockQueue::push_front() exit";
        }
    }

	Task* pop_front(){		// todo need pop  in this 
		Task* t = NULL;
		ldd::util::MutexLock l(&mutex_);
		if (destroy_ == true){
			return t;
		}

		while (task_queue_.empty() == true && destroy_ == false){
			DLOG(INFO)<<"BlockQueue::Waiting()";
			cond_.Wait();
		}

		if (task_queue_.size() > 0){
			t = task_queue_.front();
			task_queue_.pop_front();
		}
		DLOG(INFO)<<"BlockQueue::front(), exit";
		return t;
	}

private:
	std::deque<Task*>	task_queue_;
	ldd::util::Mutex	mutex_;
	ldd::util::CondVar	cond_;
	bool				destroy_;
    int                 seq_;
};


class WriteTaskQueue : public BlockQueue{
public:
	WriteTaskQueue(){

	}

private:
	
};

class ReadTaskQueue : public BlockQueue{
public:
	ReadTaskQueue(){

	}

private:

};





}//namespace storage
}//namespace ldd


#endif //__LDD__TAST_QUEUE_H_
