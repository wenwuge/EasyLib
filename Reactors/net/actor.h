#ifndef __EVENTLOOP
#define __EVENTLOOP
#include <poller.h>
#include <boost/scoped_ptr.hpp>
#include <channel.h>
#include <Timestamp.h>
#include <boost/function.hpp>
#include <vector>
#include <Mutex.h>

using namespace std;
using namespace muduo;
typedef boost::function<void()> functor;

class Actor{
public:
    Actor();
    ~Actor();
    
    void UpdateChannel(Channel* channel);
    void RemoveChannel(Channel* channel);
    void Loop();
    void DoPendingFunctors();
    void QueueInActor(const functor &func);
    void SetPending(){pending_ = true;};

private:
    bool quit_;
    bool looping_;
    Timestamp poll_returntime_;
    bool pending_;
private:
    boost::scoped_ptr<Poller> poller_;
    list<Channel*> active_channels_;
    Channel* current_active_channel_;
    vector<functor> queue_;
    MutexLock queue_lock_;
};

#endif
