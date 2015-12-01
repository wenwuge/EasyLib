// main.cc (2013-06-23)
// Yan Gaofeng (yangaofeng@360.cn)
#include <unistd.h>
#include <pthread.h>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <glog/logging.h>

static boost::asio::io_service g_io_service;
static boost::asio::io_service::work g_work(g_io_service);
static boost::asio::deadline_timer g_timer(g_io_service);

void timer_handler(const boost::system::error_code &error)
{
    if (error) {
        LOG(ERROR) << "errmsg=" << error.message();
        return;
    }

    LOG(INFO) << "timer timeout";

    return;
}

void *worker(void *arg)
{
    g_io_service.run();

    return NULL;
}

int main()
{
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, worker, NULL);
    if (ret != 0) {
        LOG(FATAL) << "pthread create failed";
        return -1;
    }

    std::size_t remain_timers;

    remain_timers = g_timer.expires_from_now(boost::posix_time::milliseconds(3000));
    LOG(INFO) << "1, remain_timers = " << remain_timers;
    g_timer.async_wait(
            boost::bind(timer_handler, boost::asio::placeholders::error));

    //LOG(INFO) << "sleep 2 s";
    //sleep(2);

    remain_timers = g_timer.expires_from_now(boost::posix_time::milliseconds(5000));
    LOG(INFO) << "2, remain_timers = " << remain_timers;
    g_timer.async_wait(
            boost::bind(timer_handler, boost::asio::placeholders::error));

    LOG(INFO) << "sleep 3 s";
    sleep(3);

    remain_timers = g_timer.expires_from_now(boost::posix_time::milliseconds(3000));
    LOG(INFO) << "3, remain_timers = " << remain_timers;
    g_timer.async_wait(
            boost::bind(timer_handler, boost::asio::placeholders::error));

    pthread_join(tid, NULL);

    return 0;
}
