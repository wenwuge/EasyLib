#ifndef __TIMER_QUEUE
#define __TIMER_QUEUE
#include <sys/timerfd.h>
#include <map>
#include <vector>
#include <Timestamp.h>
#include <Atomic.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <list>
#include "channel.h"
class Actor;
using namespace std;
using namespace muduo;
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
        return id;
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


#endif
