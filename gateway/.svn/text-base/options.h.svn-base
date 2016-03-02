// options.h (2014-02-12)
// Yan Gaofeng (yangaofeng@360.cn)

#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <ldd/skel/module.h>

namespace lcs {
namespace gateway {
namespace option {

struct Zookeeper
{
    Zookeeper() :
      timeout(3000)
      , retry_times(3)
      , retry_interval(200)
      , init_timeout(3000) {}

    bool Init(ldd::skel::SimpleModule::ModuleCtx* ctx);

    std::string host;
    std::string root_path;

    int timeout;
    int retry_times;
    int retry_interval;
    int init_timeout;
};

struct User
{
    User()
    : first_packet_timeout(1000)
      , max_connects(5)
      , request_limit_peer_second(100)
      , pulse_relaxed_checking(false)
      , pulse_lazy_emitting(false)
      , pulse_interval(30000)
      , retry_times(3) {}

    bool Init(ldd::skel::SimpleModule::ModuleCtx* ctx);

    int first_packet_timeout; //客户端第一个数据包超时时间，单位毫秒，默认为1000 ms
    int max_connects;  //每IP连接数限制，默认为5
    int request_limit_peer_second;
    bool pulse_relaxed_checking;
    bool pulse_lazy_emitting;
    int pulse_interval;
    int retry_times;
};

struct Server
{
    Server() :
      //, request_send_timeout(50)
      request_recv_timeout(200)
      , pulse_interval(100000)
      , pulse_relaxed_checking(true)
      , pulse_lazy_emitting(true) {}

    bool Init(ldd::skel::SimpleModule::ModuleCtx* ctx);

    int request_send_timeout;
    int request_recv_timeout;
    int pulse_interval; //server timeout
    bool pulse_relaxed_checking;
    bool pulse_lazy_emitting;
};

struct Push
{
    Push() :
      send_timeout(10)
      , recv_timeout(1000)
      , once_push_users(100)
      , response_threshold(1000)
      , response_interval(50)
      , qlog_info(true)
    {}

    bool Init(ldd::skel::SimpleModule::ModuleCtx* ctx);

    int send_timeout;
    int recv_timeout;
    int once_push_users;
    int response_threshold;
    int response_interval;
    bool qlog_info;
};

struct Session
{
    Session() :
        send_timeout(100)
        , recv_timeout(300)
        , user_check_interval(1000)
        , user_num_once_send(100)
        , max_retry_counts(3) {}

    bool Init(ldd::skel::SimpleModule::ModuleCtx* ctx);

    int send_timeout;
    int recv_timeout;
    int user_check_interval;
    int user_num_once_send;
    int max_retry_counts;
};

struct Test
{
    Test() :
        enable(false)
        , server("127.0.0.0")
        , port(9527) {}

    bool Init(ldd::skel::SimpleModule::ModuleCtx* ctx);

    bool enable;
    std::string server;
    int port;
};

struct Gateway
{
    Gateway() : 
      thread_count(1)
      , threads_dispatching(true)
      , max_connections(0)
      , echo(false)
      , http_port(8000)
      , once_push_users(100)
      , use_login(true) {}

    bool Init(ldd::skel::SimpleModule::ModuleCtx* ctx);

    int thread_count; //线程数量
    bool threads_dispatching;
    uint64_t max_connections;
    bool echo;
    int http_port;
    int once_push_users;
    bool use_login;
    std::string log_conf;
    std::string status_file;
    std::string ip_locate_file;
};

} /*option*/
} /*gateway*/
} /*lcs*/

#endif

