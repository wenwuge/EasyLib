// event_loop_impl.cc (2013-08-08)
// Li Xinjie (xjason.li@gmail.com)

#include <unistd.h>
#include <glog/logging.h>
#include <event2/event.h>
#include <event2/util.h>
#include <boost/bind.hpp>
#include "event_loop_impl.h"


namespace ldd {
namespace net {

#ifdef RES_COUNTER
util::Atomic<uint64_t> EventLoop::Impl::next_id_;
util::Atomic<uint64_t> EventLoop::Impl::free_id_;
#endif

EventLoop::Impl::Impl()
    : read_fd_(fd_[0])
      , write_fd_(fd_[1])
      , tid_(0)
{
    event_base_ = event_base_new();
    CHECK_NOTNULL(event_base_);

    Init();
}

EventLoop::Impl::Impl(struct ::event_base *base)
    : event_base_(base),
      read_fd_(fd_[0]), 
      write_fd_(fd_[1]),
      tid_(0)
{
    CHECK_NOTNULL(event_base_);

    Init();
}

EventLoop::Impl::~Impl()
{
    if (read_ev_ != NULL) {
        event_del(read_ev_);
        event_free(read_ev_);
    }

    evutil_closesocket(write_fd_);
    evutil_closesocket(read_fd_);

    if (event_base_ != NULL) {
        event_base_free(event_base_);
    }
}

void EventLoop::Impl::Init(void)
{
    //int rc = pipe(fd_);
    int rc = socketpair(AF_UNIX, SOCK_STREAM, 0, fd_);
    CHECK_NE(rc, -1);

    int buff_size = 64*1024*1024;
    //LOG(INFO) << "buff size: " << buff_size;
    rc = setsockopt(write_fd_, SOL_SOCKET, SO_SNDBUF, &buff_size, sizeof(int));
    CHECK_NE(rc, -1);

    rc = evutil_make_socket_nonblocking(fd_[0]);
    CHECK_NE(rc, -1);
    rc = evutil_make_socket_nonblocking(fd_[1]);
    CHECK_NE(rc, -1);

    read_ev_ = event_new(event_base_
            , read_fd_
            , EV_READ | EV_PERSIST
            , PipeNotify
            , (void *)this);

    CHECK_NOTNULL(read_ev_);
}


void EventLoop::Impl::Run() {

    tid_ = pthread_self();

    int rc = event_add(read_ev_, NULL);
    if (rc != 0) {
        LOG(FATAL) << "event_add error";
        return;
    }

    //same as event_base_loop(event_base_, EVLOOP_NO_EXIT_ON_EMPTY)
    rc = event_base_dispatch(event_base_);
    if (rc == 1) {
        LOG(ERROR) << "event_base_dispatch error: no event registered";
        return;
    }
    else if (rc == -1) {
        LOG(FATAL) << "event_base_dispatch error";
        return;
    }
}

void EventLoop::Impl::Stop() {
    LOG(INFO) << "event loop ready to stop, tid: " << pthread_self();
    RunInLoop(boost::bind(&EventLoop::Impl::RealStop, this));
}

void EventLoop::Impl::RealStop() {

    LOG(INFO) << "event loop stop, tid: " << pthread_self();

    CHECK(event_del(read_ev_) == 0);
    CHECK(event_base_loopbreak(event_base_) == 0);

    //check read_fd_ read buffer
    while (true) {

        Functor* fn[32] = {0};

        ssize_t len = read(read_fd_, fn, sizeof(fn));
        if (len <= 0) {
            DLOG(INFO) << "no data to read from pipe";
            break;
        }

        CHECK_EQ(len % sizeof(fn[0]), 0U);
        int n = len / sizeof(fn[0]);
#ifdef RES_COUNTER
        free_id_ += n;
#endif
        DLOG(INFO) << "read " << n << " object from pipe in real stop";

        for (int i = 0; i < n; ++i) {
            (*fn[i])();
            delete fn[i];
        }
    }
}


void EventLoop::Impl::AfterFork() {
    int rc = event_reinit(event_base_);
    CHECK(rc == 0) << "event_reinit" << rc;
}

void EventLoop::Impl::RunInLoop(const Functor& functor) {
    pthread_t cur_tid = pthread_self();
    if (cur_tid == tid_) {
        functor();
    } else {
        QueueInLoop(functor);
    }
}

void EventLoop::Impl::QueueInLoop(const Functor& functor) {
    Functor *fn = new Functor(functor);
    CHECK_NOTNULL(fn);
do_send:
    ssize_t len = write(write_fd_, &fn, sizeof(fn));
    if (len != sizeof(fn)) {
        if (len < 0) {
            if (errno == EINTR) {
                goto do_send;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {

                int buff_size = 0;
                socklen_t opt_len = sizeof(int);
                int rc = getsockopt(write_fd_, SOL_SOCKET, SO_SNDBUF, &buff_size, &opt_len);
                CHECK_NE(rc, -1);

                LOG(FATAL) << "send buffer of unix domain socket is full, send buffer size: " << buff_size;
                goto do_send;
            } else {
                LOG(FATAL) << "write pipe: " << strerror(errno);
            }
        } else {
            LOG(FATAL) << "write full pipe";
        }
    }

#ifdef RES_COUNTER
    next_id_++;
#endif
}

void EventLoop::Impl::PipeNotify(int fd, short what, void *arg)
{
    Functor* fn[32] = {0};

    ssize_t len = read(fd, fn, sizeof(fn));
    LOG_IF(FATAL, len < 0) << "read pipe: " <<  strerror(errno);
    CHECK_EQ(len % sizeof(fn[0]), 0U);
    int n = len / sizeof(fn[0]);
#ifdef RES_COUNTER
    free_id_ += n;
#endif
    for (int i = 0; i < n; ++i) {
        (*fn[i])();
        delete fn[i];
    }
}

} // namespace net
} // namespace ldd

