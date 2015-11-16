#include "EventLoopThreadPool.h"
#include <sstream> 
#include <string>
#include "../common/atomic_op.h"
EventLoopThreadPool::EventLoopThreadPool(EventLoop* base_loop):base_loop_(NULL), started_(false),threads_num_(0),next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::SetThreadNum(int thread_num)
{
    threads_num_ = thread_num;
}

bool EventLoopThreadPool::Start(bool wait_until_thread_started)
{
    if(started_)
        return true;

    for(int i = 0; i< threads_num_; i++){
       ostringstream tmp;
       tmp <<"EventLoopThreadPool-thread-" << i ;
       EventLoopThreadPtr thread_ptr(new EventLoopThread);

       if(!thread_ptr->Start(wait_until_thread_started)){
           cout <<"thread " << i << " start failed" <<endl;
           return false;
       }

       thread_ptr->SetName(tmp.str());
       threads_.push_back(thread_ptr);
    }
    started_ = true;
    return true;
}

void EventLoopThreadPool::Stop(bool wait_thread_exit)
{

    for(int i = 0; i < threads_num_; i++){
        threads_[i]->Stop(wait_thread_exit);
    }
    
    if(wait_thread_exit){
        while(1){
            for(int i =0; i< threads_num_; i++){
                if(!threads_[i]->IsStopped()){
                    usleep(1000);
                    break;
                }
            }
            break;
        }

    }

    threads_.clear();
}

EventLoop* EventLoopThreadPool::GetNextLoop()
{
    EventLoop* loop = base_loop_;

    if (!threads_.empty())
    {
        /**
        * No lock here
        */
        unsigned int next = InterlockedInc32(&next_);
        next = next % threads_.size();
        loop = (threads_[next])->GetEventLoop().get();
    }
    return loop;
}

EventLoop* EventLoopThreadPool::GetNextLoopWithHash(uint64_t hash)
{
    EventLoop* loop = base_loop_;

    if (!threads_.empty())
    {
        uint64_t next = hash % threads_.size();
        loop = (threads_[next])->GetEventLoop().get();
    }
    return loop;
}

bool EventLoopThreadPool::IsRunning()const
{
    for (int i = 0; i < threads_num_; ++i)
    {
        const EventLoopThreadPtr& t = threads_[i];
        if (!t->IsRunning())
        {
            return false;
        }
    }

    return true;
}

bool EventLoopThreadPool::IsStopped()const
{
    for (int i = 0; i < threads_num_; ++i)
    {
        const EventLoopThreadPtr& t = threads_[i];
        if (!t->IsStopped())
        {
            return false;
        }
    }

    return true;
}
