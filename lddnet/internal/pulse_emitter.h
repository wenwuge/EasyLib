// pulse_emitter.h (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_INTERNAL_PULSE_EMITTER_H_
#define LDD_NET_INTERNAL_PULSE_EMITTER_H_

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include "ldd/net/event.h"

namespace ldd {
namespace net {

class PulseEmitter : boost::noncopyable {
public:
    typedef boost::function<void()> Pulser;
    PulseEmitter(EventLoop* event_loop, const Pulser& pulser,
            const util::TimeDiff& interval)
        : timer_(event_loop), pulser_(pulser), interval_(interval),
          running_(false) {}
    virtual ~PulseEmitter();
    void Start(bool emit = false);
    void Stop();
    bool running() const { return running_; }

    void NotifyIncomingPulse();
    void NotifyIncomingPacket();
    void NotifyOutgoingPacket();
protected:
    virtual void DoStart() = 0;
    virtual void IncomingPulse() = 0;
    virtual void IncomingPacket() = 0;
    virtual void OutgoingPacket() = 0;
    void Pulse() { pulser_(); }
    const util::TimeDiff& interval() const { return interval_; }
    TimerEvent& timer() { return timer_; }
private:
    TimerEvent timer_;
    Pulser pulser_;
    util::TimeDiff interval_;
    bool running_;
};

class ClockPulseEmitter : public PulseEmitter {
public:
    ClockPulseEmitter(EventLoop* event_loop, const Pulser& pulser,
            const util::TimeDiff& interval)
        : PulseEmitter(event_loop, pulser, interval) {}
    virtual void DoStart();
    virtual void IncomingPulse() {}
    virtual void IncomingPacket() {}
    virtual void OutgoingPacket() {}
private:
    void DoPulse();
};

class ReplyPulseEmitter : public PulseEmitter {
public:
    ReplyPulseEmitter(EventLoop* event_loop, const Pulser& pulser,
            const util::TimeDiff& interval)
        : PulseEmitter(event_loop, pulser, interval) {}
    virtual void DoStart();
    virtual void IncomingPulse();
    virtual void IncomingPacket() {}
    virtual void OutgoingPacket() {}
};

class LazyPulseEmitter : public PulseEmitter {
public:
    LazyPulseEmitter(EventLoop* event_loop, const Pulser& pulser,
            const util::TimeDiff& interval)
        : PulseEmitter(event_loop, pulser, interval) {}
    virtual void DoStart();
    virtual void IncomingPulse() {}
    virtual void IncomingPacket() {}
    virtual void OutgoingPacket();
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_INTERNAL_PULSE_EMITTER_H__
