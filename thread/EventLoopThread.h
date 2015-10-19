#ifndef __EVENTLOOPTHREAD
#define __EVENTLOOPTHREAD
#include "Thread.h"
#include "EventLoop.h"

class EventLoopThread{
    public:
        typedef std::tr1::function<void()> functor;
        EventLoopThread();
        ~EventLoopThread();

        bool Start(bool wait_until_thread_started=false);
        void Stop(bool wait_until_thread_exited=false);
        void SetName(string name);
        string GetName();
        pthread_t Tid();    
        struct event_base* GetEventBase();
        std::tr1::shared_ptr<EventLoop> GetEventLoop();
        bool IsRunning();
        bool IsStopped();
    private:
        void Run();

    private:
        std::tr1::shared_ptr<EventLoop> event_loop_ptr_;
        std::tr1::shared_ptr<ThreadRunner> thread_ptr_;         

};
#endif
