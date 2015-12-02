// main.cc (2013-07-02)
// Yan Gaofeng (yangaofeng@360.cn)
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <glog/logging.h>
#include <sys/ioctl.h>

#include <string.h>
#include <errno.h>
#include <glog/logging.h>
#include <boost/bind.hpp>
#include "ldd/net/event_loop.h"
#include "ldd/net/event.h"
#include "ldd/util/timestamp.h"

using namespace ldd;
using namespace ldd::net;
using namespace ldd::util;

void func()
{
    //LOG(INFO) << "func called";
}

void set_noblocking(int fd)
{
    unsigned long nonblocking = 1;
    ioctl(fd, FIONBIO, (void *)&nonblocking);
}

void SendMsg(int fd, const std::string &msg)
{
    int ret = write(fd, msg.data(), msg.size());
    if (ret == -1) {
        //LOG(FATAL) << "write failed, errmsg=" << strerror(errno);
    } else {
        //LOG(INFO) << "write string=" << msg;
    }
}

void HandleTimer() {
    fprintf(stderr, "HandleTimer\n");
}

void HandleRead(int fd, int event) {
    fprintf(stderr, "HandleRead fd=%d event=%d\n", fd, event);
}

EventLoop* g_loop = NULL;

void HandleSignal(int sig) {
    fprintf(stderr, "handleSignal %d", sig);
    g_loop->Stop();
}

void HandleStop() {
    g_loop->Stop();
}

int main(int argc, char *argv[])
{
    //google::InitGoogleLogging(argv[0]);
    FLAGS_stderrthreshold = 0;
    int fd[2];
    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
    if (ret == -1) {
        //LOG(FATAL) << "socketpair error, errmsg=" << strerror(errno);
        return -1;
    }

    //set_noblocking(fd[0]);
    //set_noblocking(fd[1]);

    g_loop = new EventLoop();
    SignalEvent* sig = new SignalEvent(g_loop);
    sig->Add(SIGTERM);
    sig->Add(SIGINT);
    sig->Add(SIGQUIT);
    sig->AsyncWait(boost::bind(&HandleSignal, _1));

    TimerEvent* timer = new TimerEvent(g_loop);
    timer->AsyncWait(HandleTimer, util::TimeDiff::Seconds(1));

    FdEvent* fdevent = new FdEvent(g_loop);
    fdevent->AsyncWait(fd[1], FdEvent::kReadable,
            boost::bind(&HandleRead, fd[1], _1),
            TimeDiff::Milliseconds(3000));

    TimerEvent* timer2 = new TimerEvent(g_loop);
    timer2->AsyncWait(HandleStop, util::Timestamp::Now() + util::TimeDiff::Seconds(5));

    SendMsg(fd[0], "hello");
    g_loop->Run();
    delete fdevent;
    delete timer2;
    delete timer;
    delete sig;
    delete g_loop;
    return 0;
}

