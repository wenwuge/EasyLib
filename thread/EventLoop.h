#ifndef __EVENTLOOP
#define __EVENTLOOP
#include <iostream>
#include <pthread.h>
#include <vector>
#include <event2/event.h>
#include <tr1/functional>
#include <tr1/memory>
#include <unistd.h>
#include "easyevent.h"
using namespace std;
class EventLoop{
    public:
        typedef std::tr1::function<void()> functor;
        EventLoop();
        ~EventLoop();
        void Run();
        void Stop();
        void RunInLoop(const functor & fun);
        void QueueInLoop(const functor & fun);
        struct event_base* EventBase();
    private:
        void PipeProcess(int fd, short which, void* arg);
        void Notify();
        void StopInLoop();
        void DoPendingFunctors();
    private:
        struct event_base* base_;
        vector<functor> functors_queue_;
        pthread_t tid_;
        pthread_mutex_t mutex_;
        
        std::tr1::shared_ptr<FdEvent> pipe_event_;
        bool calling_pending_functors_;
        int32_t pipes_[2];
};

#endif
