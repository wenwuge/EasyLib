#include "eventloopthread.h"
#include <sys/eventfd.h>

EventLoopThread::EventLoopThread(string& name)
    :thread_(new Thread(boost::bind(&EventLoopThread::ThreadLoopFunc, this), name)),
     actor_(new Actor()), cond_(mutex_)
{
    
    int notify_fd_ = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (notify_fd_ < 0)
    {
        cout << "Failed in eventfd";
        abort();
    }
}

void EventLoopThread::Run()
{

    //add notify_fd_ into local actor
    notify_channel_.reset(new Channel(actor_.get(), notify_fd_));
    notify_channel_->setReadCallback(boost::bind(&EventLoopThread::ReadNotifyEvents, this));
    notify_channel_->enableReading();

    //start thread
    thread_->start();

    {
        MutexLockGuard lock(mutex_);
        while(state_ != STARTED){
            cond_.wait();
        }
    }
}

void EventLoopThread::ThreadLoopFunc()
{
    {
        MutexLockGuard lock(mutex_);
        state_ = STARTED;
        cond_.notify();
    }
    pid_ = CurrentThread::tid();
    //here ,maybe the actor will poll
    actor_->Loop();
}


void EventLoopThread::ReadNotifyEvents()
{
}

EventLoopThread::~EventLoopThread()
{
}

bool EventLoopThread::IsInLoop()
{
    return thread_->tid() == CurrentThread::tid();
}

void EventLoopThread::NotifyEvent()
{
    uint64_t one = 1;
    int n = write(notify_fd_ , &one ,sizeof(one));
    if(n != sizeof(one)){
        cout << "notify evnet loop failed" << endl;
    }
}

void EventLoopThread::QueueInLoop(functor & func)
{
    if(IsInLoop()){
        func();
    }else{
        MutexLockGuard guard(queue_lock_);
        queue_.push_back(func);
        //notify the relative loop thread
        
    }
}
