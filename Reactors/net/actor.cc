#include "actor.h"
const int kPollTimeMs = 10000;
Actor::Actor()
    :poller_(Poller::newDefaultPoller(this))
{
}

Actor::~Actor()
{

}

void Actor::DoPendingFunctors()
{
   vector<functor> tmp;
   {
        MutexLockGuard guard(queue_lock_);
        swap(tmp, queue_);

   }
   cout << "tmp size is " << tmp.size() << endl;
   for(int i = 0; i < tmp.size(); i++){
       tmp[i]();
   }
   cout << "finish do pending functor" << endl;
   pending_ = false;
}

void Actor::UpdateChannel(Channel* channel)
{
    poller_->UpdateChannel(channel);
}

void Actor::RemoveChannel(Channel* channel){
    poller_->RemoveChannel(channel);
}

void Actor::QueueInActor(const functor &func)
{
    MutexLockGuard guard(queue_lock_);
    queue_.push_back(func);
}

void Actor::Loop()
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
        if(pending_)
            DoPendingFunctors();
    }

    looping_ = false;
}
