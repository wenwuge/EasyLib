// log_flusher.h (2013-08-29)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_SKEL_INTERNAL_LOG_FLUSHER_H_
#define LDD_SKEL_INTERNAL_LOG_FLUSHER_H_

#include <ldd/net/event.h>
#include <ldd/net/event_loop.h>

namespace ldd {
namespace skel {

class LogFlusher {
public:
    LogFlusher(net::EventLoop* event_loop, int seconds, bool start = true);
    ~LogFlusher();

    void Start();
    void Stop();

    void set_seconds(int seconds) { seconds_ = seconds; }
private:
    void Flush();
    net::TimerEvent timer_;
    int seconds_;
};

} // namespace skel
} // namespace ldd

#endif // LDD_SKEL_INTERNAL_LOG_FLUSHER_H_
