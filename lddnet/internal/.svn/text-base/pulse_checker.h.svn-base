// pulse_checker.h (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_INTERNAL_PULSE_CHECKER_H_
#define LDD_NET_INTERNAL_PULSE_CHECKER_H_

#include <boost/function.hpp>
#include "ldd/net/event.h"

namespace ldd {
namespace net {

class EventLoop;

class PulseChecker {
public:
    typedef boost::function<void()> Reporter;

    PulseChecker(EventLoop* event_loop, const Reporter& reporter,
            const util::TimeDiff& interval);
    virtual ~PulseChecker() {}
    void Start();
    void Stop();
    void NotifyIncomingPulse();
    void NotifyIncomingPacket();
    bool running() const { return running_; }
protected:
    virtual void IncomingPulse() = 0;
    virtual void IncomingPacket() = 0;
    void Report() const { reporter_(); }
    const util::TimeDiff& interval() const { return interval_; }
    TimerEvent& timer() { return timer_; }
private:
    TimerEvent timer_;
    Reporter reporter_;
    util::TimeDiff interval_;
    bool running_;
};

class StrictPulseChecker : public PulseChecker {
public:
    StrictPulseChecker(EventLoop* event_loop, const Reporter& reporter,
            const util::TimeDiff& interval)
        : PulseChecker(event_loop, reporter, interval) {}
    virtual void IncomingPulse();
    virtual void IncomingPacket() {
        DLOG(INFO) << "enter IncomingPacket, nothing to do in StrictPulseChecker";
    }
};

class RelaxedPulseChecker : public StrictPulseChecker {
public:
    RelaxedPulseChecker(EventLoop* event_loop, const Reporter& reporter,
            const util::TimeDiff& interval)
        : StrictPulseChecker(event_loop, reporter, interval) {}
    virtual void IncomingPacket() {
        DLOG(INFO) << "enter IncomingPacket, call NotifyIncomingPulse in RelaxedPulseChecker";
        NotifyIncomingPulse();
    }
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_INTERNAL_PULSE_CHECKER_H_
