// pulse_checker.cc (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/bind.hpp>
#include <glog/logging.h>
#include "pulse_checker.h"

namespace ldd {
namespace net {

PulseChecker::PulseChecker(EventLoop* event_loop, const Reporter& reporter,
        const util::TimeDiff& interval)
    : timer_(event_loop),
      reporter_(reporter),
      interval_(interval),
      running_(false)
{
}

void PulseChecker::Start() {
    CHECK(!running());
    running_ = true;
    util::TimeDiff td = interval() * 2;
    DLOG(INFO) << "PulseChecker call start, pulse interval: "
        << td.ToMilliseconds();

    timer().AsyncWait(
            boost::bind(&PulseChecker::Report, this),
            interval() * 2);
}

void PulseChecker::Stop() {
    CHECK(running());
    running_ = false;

    timer().Cancel();
}

void PulseChecker::NotifyIncomingPulse() {
    if (running()) {
        IncomingPulse();
    }
}

void PulseChecker::NotifyIncomingPacket() {
    if (running()) {
        IncomingPacket();
    }
}

void StrictPulseChecker::IncomingPulse() {
    util::TimeDiff td = interval() * 2;
    DLOG(INFO) << "StrictPulseChecker call IncomingPulse, interval: " 
        << td.ToMilliseconds();
    timer().AsyncWait(
            boost::bind(&StrictPulseChecker::Report, this),
            interval() * 2);
}

} // namespace net
} // namespace ldd
