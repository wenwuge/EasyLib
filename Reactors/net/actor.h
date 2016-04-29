#ifndef __EVENTLOOP
#define __EVENTLOOP
#include <poller.h>
#include <boost/scoped_ptr.hpp>
#include <channel.h>
#include <Timestamp.h>

using namespace muduo;
class Actor{
public:
    Actor();
    ~Actor();
    
    void UpdateChannel(Channel* channel);
    void RemoveChannel(Channel* channel);
    void Loop();
    void DoPendingFunctors();

private:
    bool quit_;
    bool looping_;
    Timestamp poll_returntime_;
private:
    boost::scoped_ptr<Poller> poller_;
    list<Channel*> active_channels_;
    Channel* current_active_channel_;
};

#endif
