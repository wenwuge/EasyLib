#include "poller.h"
#include "pollers/selectpoller.h"
#include "pollers/epoller.h"


Poller* Poller::newDefaultPoller(Actor* actor)
{
#ifdef USE_EPOLL
    return new EpollPoller(); 
#else
    return new SelectPoller(); 
#endif
}
