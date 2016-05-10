#include "eventloopthread.h"
#include <sys/eventfd.h>
#include <errno.h>

EventLoopThread::EventLoopThread(string& name)
    :thread_(new Thread(boost::bind(&EventLoopThread::ThreadLoopFunc, this), name)),
     actor_(new Actor()), cond_(mutex_)
{
    
    notify_fd_ = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    cout << "notify_fd_ is " << notify_fd_ << endl;
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
    int cnt = 0;
    uint64_t one;

    if((cnt = read(notify_fd_, &one, sizeof(one))) != sizeof(one) ){
        cout << "read from notify_fd_ "<< notify_fd_ <<" error, cnt" << cnt <<
         " errno: " << errno <<endl;
    }else{
        actor_->SetPending();
    }
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
    cout << "write into notify_fd_ " << endl;
    if(n != sizeof(one)){
        cout << "notify evnet loop failed" << endl;
    }
}
void EventLoopThread::QueueInLoop(const functor & func)
{
    if(IsInLoop()){
        func();
    }else{

        actor_->QueueInActor(func);
        //notify the relative loop thread
        NotifyEvent();
        
    }
}
