#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* base_loop):base_loop_(NULL), started_(false),threads_num_(0),next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::SetThreadNum(int thread_num)
{
    thread_num_ = thread_num;
}

bool EventLoopThreadPool::Start(bool wait_until_thread_started)
{
    if(started_)
        return true;

    for(int i = 0; i< threads_num_; i++){
       osstreamstring tmp;
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
}

EventLoop* EventLoopThreadPool::GetNextLoop()
{
}

EventLoop* EventLoopThreadPool::GetNextLoopWithHash(uint64_t hash)
{
}

bool EventLoopThreadPool::IsRunning()const
{
}

bool EventLoopThreadPool::IsStopped()const
{
}
