#include "channel.h"
#include <boost/shared_ptr.hpp>
#include <actor.h>

void Channel::Remove()
{
    actor_->RemoveChannel(this);
}

void Channel::Update()
{
    actor_->UpdateChannel(this);
}

void Channel::HandleEventWithGuard(Timestamp receiveTime)
{
  eventHandling_ = true;
  if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
  {
    if (closeCallback_) closeCallback_();
  }

  if (revents_ & POLLNVAL)
  {
  }

  if (revents_ & (POLLERR | POLLNVAL))
  {
    if (errorCallback_) errorCallback_();
  }
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
  {
    if (readCallback_) readCallback_(receiveTime);
  }
  if (revents_ & POLLOUT)
  {
    if (writeCallback_) writeCallback_();
  }
  eventHandling_ = false;
}

void Channel::handleEvent(Timestamp receiveTime)
{
    boost::shared_ptr<void> guard;
    if(tied_){
        guard = tie_.lock();
        if(guard){
            HandleEventWithGuard(receiveTime);
        }
    }else{
        HandleEventWithGuard(receiveTime);
    }
}

