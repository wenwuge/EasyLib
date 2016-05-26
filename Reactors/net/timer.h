#ifndef __TIMER
#define __TIMER
#include <sys/timerfd.h>
#include <map>
#include <vector>
#include <Timestamp.h>
#include <Atomic.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>
class Actor;
using namespace std;
typedef boost::function<void()> TimerCallback;
typedef uint64_t TimerId;
class Timer{
public:
    enum{
        TIMERINIT = 0,
        TIMERADDED = 1,
        TIMERTIGERED = 2,
        TIMERREMOVED = 3
    };
    Timer(const TimerCallback &functor, const Timestamp &ts, int interval):
            repeat_((interval > 0)?true:false), interval_(interval),
            ts_(ts),callback_(functor),state_(TIMERINIT){
        timer_id_.increment();
        id = timer_id_.get();
        
    }
        
    bool Repeat(){
        return repeat_;
    }

    const Timestamp& Expiration(){
        return ts_;
    }
    void Restart(){
        Timestamp now = Timestamp::now(); 
        
        ts_ = Timestamp(now.microSecondsSinceEpoch() + interval_ * 1000 *1000); 
    }
    uint64_t TimerId(){
        return id;;
    }

    void DoFunctor(){
        if(callback_)
            callback_();
    }
private:
    bool repeat_;
    int interval_;
    Timestamp ts_;
    TimerCallback callback_;
    uint64_t  id;

    static AtomicInt64 timer_id_;
    uint8_t state_;
};

AtomicInt64 Timer::timer_id_;


class TimerQueue{
public:
    typedef list<Timer*> TimerList;
    typedef map<Timestamp, TimerList> TimerMap;
    typedef map<uint64_t, Timer*> IdToTimerMap;
    TimerQueue(Actor * actor);
    TimerId AddTimer(const TimerCallback &functor,const  Timestamp &ts,
            int interval);
    void ncelTimer(TimerId);
    void HandleRead(Timestamp ts);
    void RemoveTimer(uint64_t timerid);
private:
    void ResetTimers(vector<Timer*> & reset_timers);
    void ResetTimerfd(const Timestamp &ts);
    timespec GetIntervalFromNow(const Timestamp &ts);
private:
    Actor * actor_;
    TimerMap queue_;
    IdToTimerMap idtotimers_;
    boost::scoped_ptr<Channel> channel_;
    int timer_fd_;

};

TimerQueue::TimerQueue(Actor * actor):actor_(actor)
{
    timer_fd_ = timerfd_create(CLOCK_REALTIME,TFD_NONBLOCK | TFD_CLOEXEC);
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
            if(queue_begin == queue_.end() || ts < queue_begin->first)
                change = true;
            
            queue_[ts].push_back(*begin);
            idtotimers_[(*begin)->TimerId()] = *begin;

            if(change){
                ResetTimerfd(ts);
            }
        }else{
            delete *begin;
        }
    }
}

void TimerQueue::HandleRead(Timestamp ts)
{
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
#endif
