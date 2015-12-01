// event_impl.cc (2013-08-08)
// Li Xinjie (xjason.li@gmail.com)

#include <glog/logging.h>
#include <event2/event.h>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>

#include "ldd/util/timestamp.h"
#include "event_impl.h"

namespace ldd {
namespace net {

using namespace boost;

#ifdef RES_COUNTER
util::Atomic<uint64_t> FdEvent::Impl::next_id_;
util::Atomic<uint64_t> FdEvent::Impl::free_id_;

util::Atomic<uint64_t> TimerEvent::Impl::next_id_;
util::Atomic<uint64_t> TimerEvent::Impl::free_id_;

util::Atomic<uint64_t> SignalEvent::Impl::next_id_;
util::Atomic<uint64_t> SignalEvent::Impl::free_id_;
#endif

FdEvent::Impl::Impl(EventLoop::Impl* loop)
    : loop_(loop)
      , ev_(NULL)
      , time_flag_(kNoTime)
      , flags_(0)
      , active_(false)
{
#ifdef RES_COUNTER
    next_id_++;
#endif
}

FdEvent::Impl::~Impl() {
    DLOG(INFO) << "FdEvent::Impl::~Impl called, call Cancel";
#ifdef RES_COUNTER
    free_id_++;
#endif
    Cancel();
}

void FdEvent::Impl::Start(int fd, int events) {

    DLOG(INFO) << "FdEvent Start called, fd: " << fd;

    if ((events & (kReadable | kWritable)) == 0 ) {
        LOG(WARNING) << "invalid events, events: " << events;
        return;
    }

    Cancel();

    if (fd >= 0) {
        short e = 0;
        if (events & kReadable) {
            DLOG(INFO) << "start with read event, fd: " << fd;
            e |= EV_READ;
        }

        if (events & kWritable) {
            DLOG(INFO) << "start with write event, fd: " << fd;
            e |= EV_WRITE;
        }

        ev_ = event_new(loop_->event_base()
                , fd
                , e /*no EV_PERSIST, because FdEvent is a once event*/
                , Notify
                , (void *)this);
        CHECK_NOTNULL(ev_);

        DLOG(INFO) << "event_new called, ev_ : " << ev_;

        if (time_flag_ == kNoTime) {
            DLOG(INFO) << "start event with no time, fd: " << fd;
            int rc = event_add(ev_, NULL);
            if (rc != 0) {
                LOG(FATAL) << "event_add error";
                return;
            }
        }
        else {
            struct timeval tv;
            if (time_flag_ == kTimeDiff) {
                time_diff_.To(&tv);
                DLOG(INFO) << "start event with time diff: " 
                    << time_diff_.ToMilliseconds()
                    << ", fd: " << fd;
            }
            else if (time_flag_ == FdEvent::Impl::kTimestamp) {
                ldd::util::TimeDiff td
                    = time_stamp_ - ldd::util::Timestamp::Now();;
                DLOG(INFO) << "start event with time stamp: " 
                    << td.ToMilliseconds()
                    << ", fd: " << fd;

                td.To(&tv);
            }
            else {
                LOG(FATAL) << "invalue time type";
                return;
            }

            int rc = event_add(ev_, &tv);
            if (rc != 0) {
                LOG(FATAL) << "event_add error";
                return;
            }
        }

        active_ = true;

        return;
    }
    else {
        LOG(FATAL) << "invalid fd: " << fd;
        return;
    }
}

void FdEvent::Impl::Cancel() {

    DLOG(INFO) << "FdEvent Cancel called";

    if (!active_) {
        //DLOG(INFO) << "FdEvent Cancel called, not active, obj: " << this;
        return;
    }

    active_ = false;

    if (ev_ != NULL) {
        DLOG(INFO) << "call event_del and event_free, ev_: " << ev_;
        int rc = event_del(ev_);
        if (rc != 0) {
            LOG(FATAL) << "event_add error";
            return;
        }
        event_free(ev_);
        ev_ = NULL;
    }
    else {
        DLOG(INFO) << "ev_ is NULL, nothing to do";
    }

    handler_ = NULL;
    time_flag_ = kNoTime;
}

void FdEvent::Impl::Notify(int fd, short what, void *arg) {

    CHECK_NOTNULL(arg);

    //DLOG(INFO) << "Notify called";

    FdEvent::Impl *self = static_cast<FdEvent::Impl *>(arg);

    if (what & EV_READ) {
        DLOG(INFO) << "read event occured, fd: " << fd;
        self->flags_ |= kReadable;
    }
    else if (what & EV_WRITE) {
        DLOG(INFO) << "write event occured, fd: " << fd;
        self->flags_ |= kWritable;
    }
    else if (what & EV_TIMEOUT) {
        DLOG(INFO) << "timeout event occured, fd: " << fd;
    }
    else {
        LOG(FATAL) << "invalid event type:" << what
            << ", fd: " << fd;
        return;
    }

    //DLOG(INFO) << "call try catch block";

    try {
        DLOG(INFO) << "active: "
            << self->active_
            << ", fd: " << fd;

        if (self->active_) {
            DLOG(INFO) << "Notify call handler, fd: " << fd;
            self->handler_(self->flags_);
        }
        else {
            DLOG(WARNING) << "FDEvent, Notify nothing to do, fd: " << fd;
        }

    } catch (const std::exception& e) {
        LOG(FATAL) << "FdEvent handler exception: " << e.what();
    } catch (...) {
        LOG(FATAL) << "FdEvent handler error";
    }

    //LOG(INFO) << "Notify completed and call cancel";
}

void FdEvent::Impl::AsyncWait(int fd, int events, const Functor& handler) {
    CHECK(handler != NULL);
    DLOG(INFO) << "FdEvent AsyncWait with no time, fd: " << fd;

    if (active_) {
        Cancel();
    }

    time_flag_ = kNoTime;
    handler_ = handler;

    Start(fd, events);
}

void FdEvent::Impl::AsyncWait(int fd, int events, const Functor& handler,
        const ldd::util::Timestamp& timeout) {
    CHECK(handler != NULL);
    DLOG(INFO) << "FdEvent AsyncWait with timestamp, fd: " << fd;

    if (active_) {
        Cancel();
    }

    time_stamp_ = timeout;
    time_flag_ = kTimestamp;
    handler_ = handler;

    Start(fd, events);
}

void FdEvent::Impl::AsyncWait(int fd, int events, const Functor& handler,
        const ldd::util::TimeDiff& timeout) {

    CHECK(handler != NULL);

    DLOG(INFO) << "FdEvent AsyncWait with timediff, fd: " << fd;
    if (active_) {
        Cancel();
    }

    time_diff_ = timeout;
    time_flag_ = kTimeDiff;
    handler_ = handler;

    Start(fd, events);
}

// TimerEvent
TimerEvent::Impl::Impl(EventLoop::Impl* loop)
    : loop_(loop)
      , timer_ev_(NULL)
      , time_flag_(kNoTime)
      , active_(false)
{
#ifdef RES_COUNTER
    next_id_++;
#endif
}

TimerEvent::Impl::~Impl() {
#ifdef RES_COUNTER
    free_id_++;
#endif
    Cancel();
}

void TimerEvent::Impl::AsyncWait(const Functor& handler,
        const ldd::util::Timestamp& timeout) {
    CHECK(handler != NULL);

    Cancel();

    time_stamp_ = timeout;
    time_flag_ = kTimestamp;
    handler_ = handler;

    DLOG(INFO) << "AsyncWait with timestamp, call start, stamp: "
        << time_stamp_.ToSeconds();
    Start();
}

void TimerEvent::Impl::AsyncWait(const Functor& handler,
        const ldd::util::TimeDiff& timeout) {

    CHECK(handler != NULL);

    Cancel();

    time_diff_ = timeout;
    time_flag_ = kTimeDiff;
    start_stamp_ = ldd::util::Timestamp::Now();
    handler_ = handler;

    DLOG(INFO) << "AsyncWait with timediff,  call start, timeout: "
        << time_diff_.ToMilliseconds();

    Start(); 
}

void TimerEvent::Impl::Start() {

    //DLOG(INFO) << "TimerEvent start called, obj: " << this;

    if (active_) {
        LOG(FATAL) << "repeated call start obj: " << this;
        return;
    }

    timer_ev_ = evtimer_new(loop_->event_base()
            , OnTimer
            , (void *)this);

    CHECK_NOTNULL(timer_ev_);

    struct timeval tv;

    if (time_flag_ == kTimeDiff) {
        time_diff_.To(&tv);
    }
    else if (time_flag_ == kTimestamp) {
        ldd::util::TimeDiff td
            = (time_stamp_ - ldd::util::Timestamp::Now());
        td.To(&tv);
    }
    else {
        LOG(FATAL) << "invalid time type";
    }

    evtimer_add(timer_ev_, &tv);

    active_ = true;
}

void TimerEvent::Impl::Cancel() {

    //LOG(INFO) << "TimerEvent Cancel called, obj: " << this;

    if (!active_) {
        //DLOG(WARNING) << "TimerEvent Cancel called, not active, obj: " << this;
        return;
    }

    active_ = false;

    if (timer_ev_ != NULL) {
        int rc = evtimer_del(timer_ev_);
        if (rc != 0) {
            LOG(FATAL) << "evtimer_del error";
            return;
        }
        event_free(timer_ev_);
        timer_ev_ = NULL;
    }

    time_flag_ = kNoTime;
    handler_ = NULL;
}

bool TimerEvent::Impl::ExpiresAt(ldd::util::Timestamp* time) {
    if (time_flag_ == kTimestamp) {
        *time = time_stamp_;
    }
    else if (time_flag_ == kTimeDiff) {
        *time = (start_stamp_ += time_diff_);
    }

    return true;
}

void TimerEvent::Impl::OnTimer(int fd, short what, void *arg) {
    CHECK_NOTNULL(arg);

    TimerEvent::Impl *self = static_cast<TimerEvent::Impl *>(arg);

    //DLOG(INFO) << "OnTimer called, obj: " 
    //    << self
    //    << ", time flag:"
    //    << self->time_flag_
    //    << ", timeout: "
    //    << (self->time_flag_ == kTimestamp 
    //    ? self->time_stamp_.ToMilliseconds() 
    //    : self->time_diff_.ToMilliseconds());

    if (!(what & EV_TIMEOUT)) {
        LOG(FATAL) << "not time out event";
        return;
    }

    self->active_ = false;
    if (self->timer_ev_ != NULL) {
        int rc = evtimer_del(self->timer_ev_);
        if (rc != 0) {
            LOG(FATAL) << "evtimer_del error";
            return;
        }
        event_free(self->timer_ev_);
        self->timer_ev_ = NULL;
    }

    self->handler_();

    return;
}

// SignalEvent
SignalEvent::Impl::Impl(EventLoop::Impl* loop)
    : loop_(loop)
      , timer_ev_(NULL)
{
#ifdef RES_COUNTER
    next_id_++;
#endif
}

SignalEvent::Impl::~Impl()
{
#ifdef RES_COUNTER
    free_id_++;
#endif
    Clear();
}

bool SignalEvent::Impl::Add(int signo) {

    std::map<int, struct event *>::iterator pos = sig_ev_.find(signo);
    if (pos != sig_ev_.end()) {
        LOG(ERROR) << "repeated add signal: " << signo;
        return false;
    }

    struct event *sig_ev = evsignal_new(loop_->event_base()
            , signo
            , OnSignal
            , (void *)this);

    CHECK_NOTNULL(sig_ev);

    std::pair<std::map<int, struct event *>::iterator, bool> ret;
    ret = sig_ev_.insert(std::make_pair(signo, sig_ev));
    if (ret.second == false) {
        LOG(ERROR) << "add signal event failed, signal no: " << signo;
        return false;
    }

    return true;
}

bool SignalEvent::Impl::Remove(int signo) {

    std::map<int, struct event *>::iterator pos = sig_ev_.find(signo);
    if (pos == sig_ev_.end()) {
        LOG(ERROR) << "remvoe signal " << signo << " failed, it is not exist";
        return false;
    }

    evsignal_del(pos->second);
    event_free(pos->second);

    sig_ev_.erase(pos);

    return true;
}

bool SignalEvent::Impl::Clear() {

    if (timer_ev_ != NULL) {
        int rc = evtimer_del(timer_ev_);
        if (rc != 0) {
            LOG(FATAL) << "evtimer_del error";
            return false;
        }
        event_free(timer_ev_);
        timer_ev_ = NULL;
    }

    std::map<int, struct event *>::iterator it;
    for (it = sig_ev_.begin(); it != sig_ev_.end(); it++) {
        evsignal_del(it->second);
        event_free(it->second);
    }

    sig_ev_.clear();

    handler_ = NULL;

    return true;
}

void SignalEvent::Impl::AsyncWait(const Functor& handler)
{
    handler_ = handler;

    std::map<int, struct event *>::iterator it;
    for (it = sig_ev_.begin(); it != sig_ev_.end(); it++) {
        int rc = evsignal_add(it->second, NULL);
        if (rc != 0) {
            LOG(FATAL) << "evsignal_add error";
            return;
        }
    }
}

void SignalEvent::Impl::AsyncWait(const Functor& handler,
        const ldd::util::Timestamp& timeout)
{
    ldd::util::TimeDiff time_diff 
        = (timeout - ldd::util::Timestamp::Now());
    struct timeval tv;
    time_diff.To(&tv);

    timer_ev_ = evtimer_new(loop_->event_base()
            , OnTimer
            , (void *)this);
    CHECK_NOTNULL(timer_ev_);

    handler_ = handler;
    evtimer_add(timer_ev_, &tv);

    AsyncWait(handler);
}

void SignalEvent::Impl::AsyncWait(const Functor& handler,
        const ldd::util::TimeDiff& timeout)
{
    struct timeval tv;
    timeout.To(&tv);
    timer_ev_ = evtimer_new(loop_->event_base()
            , OnTimer
            , (void *)this);

    CHECK_NOTNULL(timer_ev_);

    handler_ = handler;
    evtimer_add(timer_ev_, &tv);

    AsyncWait(handler);
}

void SignalEvent::Impl::Cancel()
{
    Clear();
}

void SignalEvent::Impl::OnSignal(int fd, short what, void *arg)
{
    CHECK_NOTNULL(arg);

    int sig = fd;
    SignalEvent::Impl *self = static_cast<SignalEvent::Impl *>(arg);

    std::map<int, struct event *>::iterator pos = self->sig_ev_.find(sig);
    if (pos == self->sig_ev_.end()) {
        LOG(ERROR) << "recv unregistered signal: " << sig;
        return;
    }

    self->handler_(sig);
}

void SignalEvent::Impl::OnTimer(int fd, short what, void *arg)
{
    CHECK_NOTNULL(arg);

    SignalEvent::Impl *self = static_cast<SignalEvent::Impl *>(arg);

    self->handler_(0);

    self->Clear();
}

} // namespace net
} // namespace ldd

