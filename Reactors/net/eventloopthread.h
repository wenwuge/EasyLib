#ifndef __EVENTLOOPTHREAD
#define __EVENTLOOPTHREAD
#include <Thread>
#include <vector>
#include <Mutex>
#include <boost/scoped_shared.hpp>

typedef boost::function<void()> functor;

class EventLoopThread{
public:
    EventLoopThread();
    void Run();

private:
    boost::scoped_shared<Thread> thread_;
    boost::scoped_shared<Actor>  actor_;
    vector<functor> queue_;
    MutexLock queue_lock_;
    //use the notify_fd_ to notify the thread that 
    //queue has new event
    int notify_fd_;
    int watch_fd_;

};
#endif
