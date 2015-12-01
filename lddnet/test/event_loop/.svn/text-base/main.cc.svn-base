// main.cc (2014-07-16)
// Yan Gaofeng (yangaofeng@360.cn)

#include <ldd/net/event_loop.h>

#include <glog/logging.h>
#include <boost/bind.hpp>

using namespace ldd::net;

void func(int no)
{
    LOG(INFO) << "func run, no: " << no;
}

int main()
{
//测试event_loop的socketpair创建的套接字缓冲区

#if 1
    EventLoop loop;

    LOG(INFO) << "poiter size: " << sizeof(void *);
    for (int i = 0; i < 100000000; i++) {
        LOG(INFO) << "index: " << i;
        loop.QueueInLoop(boost::bind(&func, i));
    }

    return 0;

#else

    EventLoopThread loop_thread;
    EventLoop *loop = loop_thread.event_loop();
    CHECK_NOTNULL(loop);

    loop_thread.Start();

    for (int i = 0; i < 100000; i++) {
        loop->QueueInLoop(boost::bind(&func, i));
    }

    sleep(5);

    loop_thread.Stop();
    loop_thread.Join();
#endif

    return 0;
}
