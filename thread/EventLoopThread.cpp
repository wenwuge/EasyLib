#include "EventLoopThread.h"

EventLoopThread::EventLoopThread()
{
    event_loop_ptr_.reset(new EventLoop);
}

EventLoopThread::~EventLoopThread()
{
}

void EventLoopThread::Run()
{
    event_loop_ptr_->Run();    
}

bool EventLoopThread::Start(bool wait_until_thread_started)
{
    functor func = std::tr1::bind(&EventLoopThread::Run, this);

    thread_ptr_.reset(new ThreadRunner(func));
    cout << "will start thread " << endl;    
    bool rc = thread_ptr_->start();
    cout << "thread start end " << endl;
    
    if(rc == true || wait_until_thread_started == false){
        return true;
    }

    for(;;){
        if(!IsRunning()){
            usleep(100);
        }else{
            break;
        }
    }

    return true;

}

void EventLoopThread::Stop(bool wait_until_thread_exited)
{
    event_loop_ptr_->Stop();
    thread_ptr_->stop();

    while(1){
        if(IsStopped() || !wait_until_thread_exited == false){
            break;
        }
        
        sleep(1);
    }
}

void EventLoopThread::SetName(string name)
{
    thread_ptr_->setName(name);
}

string EventLoopThread::GetName()
{
    return thread_ptr_->getName();
}

std::tr1::shared_ptr<EventLoop> EventLoopThread::GetEventLoop()
{
    return event_loop_ptr_;
}

struct event_base* EventLoopThread::GetEventBase()
{
    return event_loop_ptr_->EventBase();
}

pthread_t EventLoopThread::Tid()
{
    return thread_ptr_->getCurrentThreadID();
}

bool EventLoopThread::IsRunning()
{
    return thread_ptr_->isRunning();
}

bool EventLoopThread::IsStopped()
{
    return thread_ptr_->isStopped();
}

