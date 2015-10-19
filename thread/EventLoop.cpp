#include "EventLoop.h"
#include "../common/lock.h"

EventLoop::EventLoop():tid_(0),calling_pending_functors_(false)
{ 
    base_ = event_base_new();
    pthread_mutex_init(&mutex_, NULL);
    pipe(pipes_);

    //add read pipe to libevent;
    pipe_event_.reset(new FdEvent(base_));
    pipe_event_->AsyncWait(pipes_[0], FdEvent::kReadable, std::tr1::bind(&EventLoop::PipeProcess, this,pipes_[0], FdEvent::kReadable,(void*) NULL));
}


EventLoop::~EventLoop()
{
    if(base_){
        event_base_free(base_);
    }

    close(pipes_[0]);
    close(pipes_[1]);
}

void EventLoop::Run()
{
    
    tid_ = pthread_self();
    
    int rc = event_base_dispatch(base_);
    if (rc == 1) {
        return;
    } else if (rc == -1) {
        return;
    }
}

void EventLoop::StopInLoop()
{
         
    for (;;) {
        DoPendingFunctors();
        
       // ScopedLock<pthread_mutex_t> lock(mutex_);
        pthread_mutex_lock(&mutex_);
        if (functors_queue_.empty()) {
            pthread_mutex_unlock(&mutex_);
            break;
        }
        pthread_mutex_unlock(&mutex_);

    }

    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500 * 1000; //500ms
    event_base_loopexit(base_, &tv);
}

void EventLoop::Stop()
{
    RunInLoop(std::tr1::bind(&EventLoop::StopInLoop, this));
}

void EventLoop::RunInLoop(const functor &fun)
{
    pthread_t cur_tid = pthread_self();
    if (cur_tid == tid_) {
        fun();
    }else {
        QueueInLoop(fun);
    }
}

void EventLoop::QueueInLoop(const functor &fun)
{
    {
        //ScopedLock<pthread_mutex_t> lock(mutex_);
        pthread_mutex_lock(&mutex_);
        functors_queue_.push_back(fun);
        pthread_mutex_unlock(&mutex_);
    }

    if (calling_pending_functors_ || pthread_self() != tid_)
    {
        Notify();
    }
}

void EventLoop::Notify()
{
    char buf[1] = {};
    write(pipes_[1], buf, sizeof(buf));
}

void EventLoop::PipeProcess(int fd, short which, void* arg)
{
    char buf[1] = {};
    read(pipes_[0], buf, sizeof(buf));
    DoPendingFunctors();
}

void EventLoop::DoPendingFunctors()
{
    calling_pending_functors_ = true;
    vector<functor> functors;

    {
        //ScopedLock<pthread_mutex_t> lock(mutex_);
        pthread_mutex_lock(&mutex_);
        functors.swap(functors_queue_);
        pthread_mutex_unlock(&mutex_);
    }
    
    for(int i=0; i < functors_queue_.size(); i++){
        functors_queue_[i](); 
    }

    calling_pending_functors_ = false;
   
    
}

struct event_base* EventLoop::EventBase(){
    return base_;
}
