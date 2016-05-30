#include "timer.h"

AtomicInt64 Timer::timer_id_;
TimerQueue::TimerQueue(Actor * actor):actor_(actor)
{
    timer_fd_ = timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC);
    if(timer_fd_ == -1)
        cout << "timerfd create error" << endl;
    channel_.reset(new Channel(actor_, timer_fd_));
    
    channel_->setReadCallback(boost::bind(&TimerQueue::HandleRead, this, _1));
    channel_->enableReading();
}

void TimerQueue::ResetTimers(vector<Timer*> & reset_timers)
{
    vector<Timer*>::iterator begin(reset_timers.begin()), end(reset_timers.end());  

    for(;begin != end; begin++){
        if((*begin)->Repeat()){
            (*begin)->Restart();
            //reinsert into timer queue
            bool change = false;
            TimerMap::iterator queue_begin = queue_.begin();
            Timestamp ts = (*begin)->Expiration();
            Timestamp expired_ts = ts;
            if(queue_begin != queue_.end()&&ts > queue_begin->first){
                expired_ts = queue_begin->first;
            }
            
            queue_[ts].push_back(*begin);
            idtotimers_[(*begin)->TimerId()] = *begin;

            ResetTimerfd(expired_ts);
        }else{
            delete *begin;
        }
    }
}

void TimerQueue::HandleRead(Timestamp ts)
{
    uint64_t exp;
    int s = read(timer_fd_, &exp, sizeof(uint64_t));
    if(s != sizeof(uint64_t)){
        cout << "read timer fd error" <<  endl;
    }
    Timestamp now = Timestamp::now(); 
    TimerMap::iterator begin = queue_.begin(), end = queue_.end();  
    vector<Timer*> expired;

    for(; begin != end;){
        if(begin->first.microSecondsSinceEpoch()  <= now.microSecondsSinceEpoch()){
            //expired.push_back(begin->second);
            copy(begin->second.begin() , begin->second.end(), back_inserter(expired));

            queue_.erase(begin++);
        }else
            break;
    }
    
    vector<Timer*>::iterator expired_begin = expired.begin(), expired_end = expired.end();
    for(; expired_begin != expired_end; expired_begin++){
            idtotimers_.erase((*expired_begin)->TimerId());
            (*expired_begin)->DoFunctor();
    }

    ResetTimers(expired);

}

timespec TimerQueue::GetIntervalFromNow(const Timestamp &ts)
{
   
   uint64_t interval_mseconds = ts.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();

   struct timespec spec;
   spec.tv_sec = (interval_mseconds) /(1000*1000);
   spec.tv_nsec = ((interval_mseconds)%(1000*1000))*1000 ;
   return spec;
}

void TimerQueue::ResetTimerfd(const Timestamp &ts)
{
    struct itimerspec new_value;

    new_value.it_value = GetIntervalFromNow(ts);
    if(timerfd_settime(timer_fd_, 0, &new_value,NULL) == -1){
        cout << "timerfd_settime error" << endl;
    }
}

TimerId TimerQueue::AddTimer(const TimerCallback &functor,const Timestamp &ts,
        int interval)
{
    Timer * timer = new Timer(functor, ts, interval);
    bool  change = false;
    TimerMap::iterator begin = queue_.begin();
    if(begin == queue_.end() || ts < begin->first)
        change = true;
    
    queue_[ts].push_back(timer);
    idtotimers_[timer->TimerId()] = timer;

    if(change){
        ResetTimerfd(ts);
    }

    return timer->TimerId();
    
}

void TimerQueue::RemoveTimer(uint64_t timerid)
{
    if(idtotimers_.find(timerid) != idtotimers_.end()){
        Timer * timer = idtotimers_[timerid];
        idtotimers_.erase(timerid);
        queue_[timer->Expiration()].remove(timer);
        delete timer;
        
    }
}
