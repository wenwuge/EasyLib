// log_flusher.cc (2013-08-29)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/bind.hpp>
#include <glog/logging.h>
#include "log_flusher.h"

namespace ldd {
namespace skel {

LogFlusher::LogFlusher(net::EventLoop* event_loop, int seconds, bool start)
    : timer_(event_loop), seconds_(seconds) 
{
    if (start) {
        Start();
    }
}

LogFlusher::~LogFlusher() {
    Stop();
}

void LogFlusher::Start() {
    timer_.AsyncWait(
            boost::bind(&LogFlusher::Flush, this),
            util::TimeDiff::Seconds(seconds_));
}

void LogFlusher::Flush() {
    google::FlushLogFiles(0);
    Start();
}

void LogFlusher::Stop() {
    timer_.Cancel();
}

} // namespace skel
} // namespace ldd
