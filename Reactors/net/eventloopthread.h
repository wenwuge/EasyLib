#ifndef __EVENTLOOPTHREAD
#define __EVENTLOOPTHREAD
#include <Thread.h>
#include <vector>
#include <Mutex.h>
#include <Condition.h>
#include <boost/scoped_ptr.hpp>
#include "actor.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>

using namespace std;
using namespace muduo;
typedef boost::function<void()> functor;

class EventLoopThread{
public:
    enum{
        STOPPED,
        STARTED
    };
    EventLoopThread(string& name) ;
    ~EventLoopThread();
    void Run();
    void QueueInLoop(const functor & func);
    Actor* GetActor(){
        return actor_.get();
    }

    bool IsInLoop();
private:
    void NotifyEvent();
private:
    void ThreadLoopFunc();
    void ReadNotifyEvents();private:
    boost::scoped_ptr<Thread> thread_;
    boost::scoped_ptr<Actor>  actor_;
    boost::scoped_ptr<Channel> notify_channel_;

    MutexLock mutex_;
    Condition cond_;
    //use the notify_fd_ to notify the thread that 
    //queue has new event
    int notify_fd_;
    uint8_t state_;
    pid_t pid_;

};

#endif
