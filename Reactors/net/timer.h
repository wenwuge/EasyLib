#ifndef __TIMER
#define __TIMER
class Timer{
public:
    Timer(const TimerCallback &functor, Timestamp &ts, int interval);
    void AddIntoActor();
    bool Repeat(){
        return repeat_;
    }

    void Restart(){
    }
private:
    bool repeat_;
    int interval_;
    TimerCallback callback_;
};

class TimerQueue{
public:
    typedef list<Timer*> TimerList;
    typedef map<Timestamp, TimerList> TimerMap;
    TimerQueue(Actor * actor):actor_(actor){};
    TimerId AddTimer(const TimerCallback &functor, Timestamp &ts,
            double interval);
    void ncelTimer(TimerId);

private:
    Actor * actor_;
    TimerMap queue_;
    boost::scoped_ptr<Channel> channel_;
    int timer_fd_;


};

TimerQueue::TimerQueue(Actor * actor):actor_(actor)
{
    timer_fd_ = timerfd_create(CLOCK_REALTIME,TFD_NONBLOCK | TFD_CLOEXEC);
    if(timer_fd_ == -1)
        cout << "timerfd create error" << endl;
    channel_.reset(new Channel(actor_, timer_fd_));
    
    channel_->setReadCallback(&TimerQueue::HandleRead, this);
    channel_->enalbeReading();
}

void TimeQueue::ResetTimers(vector<Timer*> & reset_timers)
{
    vector<Timer*>::iterator begin(reset_timers.begin()), end(reset_timers.end());  

    for(;begin != end; begin++){
        if((*begin)->Repeat()){
            (*begin)->Restart();
            //reinsert into timer queue
            bool change = false;
            TimerMap::iterator begin = queue_.begin();
            if(begin == queue_.end() || ts < begin->Expiration())
                change = true;
            
            queue_[ts].push_back(timer);

            if(change){
                ResetTimerfd(ts);
            }
        }else{
            delete *begin;
        }
    }
}

void TimeQueue::HandleRead()
{
    Timestamp now = Timestamp::now(); 
    TimerMap::iterator begin = queue_.begin(), end = queue_.end();  
    vector<Timer*> expired;

    for(; begin != end;){
        if(begin->first.microSecondSinceEpoch()  <= now.microSecondSinceEpoch()){
            expired.push_back(begin->second);
            queue_.erase(begin++);
        }else
            break;
    }
    
    vector<Timer*> reset_timers;
    vector<TimerList>::iterator expired_begin = expired.begin(), expired_end = expired.end();
    for(; expired_begin != expired_end; expired_begin++){
        TimerList::iterator begin = (*expired_begin).begin(), end = (*expired_begin).end();
        for(; begin != end; begin++){
            reset_timers.push_back(begin);
            (*begin)->DoFunctor();
        }
    }

    ResetTimers(reset_timers);
}

timespec TimerQueue::GetIntervalFromNow(Timestamp &ts)
{
   
   uint64_t interval_mseconds = ts.microSecondSinceEpoch() - Timestamp::now().microSecondSinceEpoch();

   struct timespec spec;
   spec.tv_sec = (interval_mseconds) /(1000*1000);
   spec.tv_nsec = ((interval_mseconds)%(1000*1000))*1000 ;
   return spec;
}

void TimerQueue::ResetTimerfd(Timestamp &ts)
{
    struct itimerspec new_value;

    new_value.it_value = GetIntervalFromNow(ts);
    if(timerfd_settime(timer_fd_, 0, &new_value,NULL) == -1){
        cout << "timerfd_settime error" << endl;
    }
}

TimerId TimerQueue::AddTimer(const TimerCallback &functor, Timestamp &ts,
        double interval)
{
    Timer * timer = new Timer(functor, ts, interval);
    bool  change = false;
    TimerMap::iterator begin = queue_.begin();
    if(begin == queue_.end() || ts < begin->Expiration())
        change = true;
    
    queue_[ts].push_back(timer);

    if(change){
        ResetTimerfd(ts);
    }

    return TimerId(timer);
    
}
#endif
