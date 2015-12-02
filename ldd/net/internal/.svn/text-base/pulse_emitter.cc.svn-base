// pulse_emitter.cc (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/bind.hpp>
#include <glog/logging.h>
#include "pulse_emitter.h"

namespace ldd {
namespace net {

void PulseEmitter::Start(bool emit) {
    CHECK(!running());
    running_ = true;
    if (emit) {
        Pulse();
    }
    DoStart();
}

PulseEmitter::~PulseEmitter() {
    timer().Cancel();
}

void PulseEmitter::Stop() {
    CHECK(running());
    running_ = false;
    timer().Cancel();
}

void PulseEmitter::NotifyIncomingPulse() {
    if (running()) {
        IncomingPulse();
    }
}

void PulseEmitter::NotifyIncomingPacket() {
    if (running()) {
        IncomingPacket();
    }
}

void PulseEmitter::NotifyOutgoingPacket() {
    DLOG(INFO) << "enter NotifyOutgoingPacket";
    if (running()) {
        DLOG(INFO) << "PulseEmitter call OutgoingPacket";
        OutgoingPacket();
    }
}

void ClockPulseEmitter::DoStart() {
    DLOG(INFO) << "ClockPulseEmitter call DoStart, interval: " 
        << interval().ToMilliseconds();
    timer().AsyncWait(
            boost::bind(&ClockPulseEmitter::DoPulse, this), interval());
}

void ClockPulseEmitter::DoPulse() {
    Pulse();
    DLOG(INFO) << "ClockPulseEmitter call DoPulse, interval: "
        << interval().ToMilliseconds();
    timer().AsyncWait(
            boost::bind(&ClockPulseEmitter::DoPulse, this), interval());
}

void ReplyPulseEmitter::DoStart() {
}

void ReplyPulseEmitter::IncomingPulse() {
    Pulse();
}

void LazyPulseEmitter::DoStart() {
    DLOG(INFO) << "LazyPulseEmitter call DoStart, interval: "
        << interval().ToMilliseconds();
    timer().AsyncWait(
            boost::bind(&LazyPulseEmitter::Pulse, this), interval());
}

void LazyPulseEmitter::OutgoingPacket() {
    DLOG(INFO) << "enter OutgoingPacket, interval: "
        << interval().ToMilliseconds();
    timer().AsyncWait(
            boost::bind(&LazyPulseEmitter::Pulse, this), interval());
}

} // namespace net
} // namespace ldd

