
#ifndef LDD_NET_EVENT_LOOP_THREAD_POOL_H_
#define LDD_NET_EVENT_LOOP_THREAD_POOL_H_

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace ldd {
namespace net {

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : private boost::noncopyable
{
public:
    EventLoopThreadPool(EventLoop* baseLoop);
    ~EventLoopThreadPool();
    void SetThreadNum(int numThreads) { threads_num_ = numThreads; }
    bool Start();
    void Stop();
    EventLoop* GetNextLoop();

public:
    boost::ptr_vector<EventLoopThread>* threads() { return &threads_; }

private:
    EventLoop* base_loop_;
    bool started_;
    int threads_num_;
    size_t next_;
    boost::ptr_vector<EventLoopThread> threads_;
};


} // namespace net
} // namespace ldd

#endif // LDD_NET_EVENT_LOOP_H_
