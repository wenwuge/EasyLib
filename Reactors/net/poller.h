#ifndef __POLLER
#define __POLLER
#include <map>
#include <channel.h>
#include <list>
using namespace std;
class Actor;
class Poller{
public:
    typedef list<Channel*> ChannelList;
    Poller(){}
    virtual Timestamp Poll(int timeout_ms, ChannelList& active_channels);
    virtual bool UpdateChannel(Channel* channel) = 0;
    virtual bool RemoveChannel(Channel* channel) = 0;
    static Poller* newDefaultPoller(Actor* actor);

protected:
    void ProcessEvent();

    typedef map<int, Channel*> ChannelMap;
    ChannelMap channels_ ;
};

#endif
