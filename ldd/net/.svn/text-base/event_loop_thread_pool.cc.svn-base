#include "ldd/net/event_loop_thread_pool.h"
#include "ldd/net/event_loop.h"

#include <glog/logging.h>

namespace ldd {
namespace net {


EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
    : base_loop_(baseLoop),
    started_(false),
    threads_num_(0),
    next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

bool EventLoopThreadPool::Start()
{
    assert(!started_);

    for (int i = 0; i < threads_num_; ++i)
    {
        EventLoopThread* t = new EventLoopThread();
        if (!t->Start()) {
            //FIXME error process
            LOG(ERROR) << "start thread failed!";
            return false;
        }
        threads_.push_back(t);
    }

    started_ = true;
    return true;
}

void EventLoopThreadPool::Stop()
{

    for (int i = 0; i < threads_num_; ++i)
    {
        EventLoopThread& t = threads_[next_];
        t.Stop();
        t.Join();
    }
    threads_.clear();
}

EventLoop* EventLoopThreadPool::GetNextLoop()
{
    EventLoop* loop = base_loop_;

    if (!threads_.empty())
    {
        /**
         * No lock here because we offen use it at the startup time
         */
        // round-robin
        loop = (threads_[next_]).event_loop();
        ++next_;
        if (next_ >= threads_.size())
        {
            next_ = 0;
        }
    }
    return loop;
}

} // namespace net
} // namespace ldd


