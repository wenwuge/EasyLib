#ifndef __WORK_H
#define __WORK_H
#include <Condition.h>
#include <Mutex.h>
#include <Thread.h>
#include <list>
#include <sstream>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <vector>

using namespace muduo;
using namespace std;
typedef boost::function<void()> Job;
class Worker{
public:
    Worker(MutexLock *lock, Condition* condition, list<Job>* jobs, uint8_t id)
        :worker_id_(id), lock_(lock),
         condition_(condition), job_list_(jobs){}

    void Work(){
        while(1){
            Job job;
            {
                MutexLockGuard guard(*lock_);
                while(job_list_->size() == 0){
                    condition_->wait();
                }

                job = job_list_->front();
                job_list_->pop_front();
            
            }
            job();
        }
    }

    Worker& operator=(const Worker& copy){
        if(&copy == this)
            return *this;
        worker_id_ = copy.worker_id_;
        lock_  = copy.lock_;
        condition_ = copy.condition_;
        job_list_ = copy.job_list_;
        thread_ptr_ = copy.thread_ptr_;
        
    }
    void Start(){
        ostringstream oss;
        oss << "worker-" << worker_id_;
        thread_ptr_.reset(new Thread(boost::bind(&Worker::Work, this), oss.str()));        
        thread_ptr_->start();
    }
private:
    uint8_t  worker_id_;
    MutexLock * lock_;
    Condition * condition_;
    list<Job> * job_list_;
    boost::shared_ptr<Thread> thread_ptr_;  
};

class WorkerManager{
public:
    WorkerManager(int worker_num):
        mutex_(), condition_(mutex_),worker_num_(worker_num){
    }
    void StartWork(){
        for(uint8_t i = 0; i < worker_num_; i++){
            workers_.push_back(Worker(&mutex_, &condition_, &job_list_, i));
            workers_[i].Start();
        }
    }
    void Stop();
    void QueueJob(const Job& job){
        MutexLockGuard guard(mutex_);
        job_list_.push_back(job);
        condition_.notify();
    }
private:
    vector<Worker> workers_;
    list<Job> job_list_;
    MutexLock mutex_;
    Condition condition_;
    uint32_t  worker_num_;
    
};


#endif
