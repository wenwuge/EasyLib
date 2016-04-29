#include "actor.h"
const int kPollTimeMs = 10000;
Actor::Actor()
    :poller_(Poller::newDefaultPoller(this))
{
}

Actor::~Actor()
{
    looping_ = true;
    quit_ = false;
    while(!quit_){
        active_channels_.clear();
        poll_returntime_ = poller_->Poll(kPollTimeMs, active_channels_);
        
        for(list<Channel*>::iterator it = active_channels_.begin(); it !=  active_channels_.end(); it++){
            current_active_channel_ = *it;
            current_active_channel_->HandleEvent(poll_returntime_);
        }

        current_active_channel_ = NULL;
        DoPendingFunctors();
    }

    looping_ = false;
}

void Actor::DoPendingFunctors()
{
}

void Actor::UpdateChannel(Channel* channel)
{
    poller_->UpdateChannel(channel);
}

void Actor::RemoveChannel(Channel* channel){
    poller_->RemoveChannel(channel);
}

void Actor::Loop()
{

}
