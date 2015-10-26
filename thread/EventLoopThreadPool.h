#ifndef __EVNET_LOOP_THREAD_POOL
#define __EVENT_LOOP_THREAD_POOL
#include "EventLoopThread.h"
#include "EventLoop.h"
#include <vector>
#include <tr1/memory>
#include "../common/atomic.h"
using namespace std;

class EventLoopThreadPool
{
public:
    EventLoopThreadPool(EventLoop* base_loop);
    ~EventLoopThreadPool();
    void SetThreadNum(int thread_num);
    bool Start(bool wait_until_thread_started = false);
    void Stop(bool wait_thread_exit = false);

public:
    EventLoop* GetNextLoop();
    EventLoop* GetNextLoopWithHash(uint64_t hash);

    bool IsRunning() const;
    bool IsStopped() const;

public:
    typedef std::tr1::shared_ptr<EventLoopThread> EventLoopThreadPtr;
    vector<EventLoopThreadPtr>* threads();

private:
    
    EventLoop* base_loop_;
    bool started_;
    int threads_num_;
    //AtomicInt32 next_;
    int next_;

    vector<EventLoopThreadPtr> threads_;
};
#endif
