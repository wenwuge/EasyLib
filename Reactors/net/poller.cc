#include "poller.h"
#include "pollers/selectpoller.h"


Poller* Poller::newDefaultPoller(Actor* actor)
{
#ifdef USE_EPOLL
    return new EpollPoller(); 
#else
    return new SelectPoller(); 
#endif
}
