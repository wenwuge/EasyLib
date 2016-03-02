// options.cc (2014-02-12)
// Yan Gaofeng (yangaofeng@360.cn)

#include <glog/logging.h>
#include <ldd/net/server.h>
#include "options.h"
#include "default_msg.h"

namespace lcs {
namespace gateway {
namespace option {

bool Zookeeper::Init(ldd::skel::SimpleModule::ModuleCtx* ctx)
{
    try {
        //zookeeper config
        host = ctx->config().get<std::string>("zookeeper.host"
                , "localhost" /*default*/);
        root_path = ctx->config().get<std::string>("zookeeper.root_path"
                , "/" /*default*/);
        timeout = ctx->config().get<int>("zookeeper.timeout"
                , 2000 /*default*/);
        retry_times = ctx->config().get<int>("zookeeper.retry_times"
                , 3 /*default*/);
        retry_interval = ctx->config().get<int>("zookeeper.retry_interval"
                , 1000 /*default*/);
        init_timeout = ctx->config().get<int>("zookeeper.init_timeout"
                , 3000 /*default*/);
    }
    catch (const boost::property_tree::ptree_bad_data &e2) {
        LOG(ERROR) << "ptree bad data: " << e2.what();
        return false;
    }
    catch (const boost::property_tree::ptree_bad_path &e3) {
        LOG(ERROR) << "ptree bad path: " << e3.what();
        return false;
    }
    catch (const boost::property_tree::ptree_error &e1) {
        LOG(ERROR) << "ptree error: " << e1.what();
        return false;
    }
    catch (...) {
        LOG(ERROR) << "unknown exception";
        return false;
    }

    LOG(INFO) << "zookeeper host: " << host;
    LOG(INFO) << "zookeeper root path: " << root_path;
    LOG(INFO) << "zookeeper timeout: " << timeout;
    LOG(INFO) << "zookeeper retry times: " << retry_times;
    LOG(INFO) << "zookeeper retry interval: " << retry_interval;
    LOG(INFO) << "zookeeper init timeout: " << init_timeout;

    return true;
}


bool User::Init(ldd::skel::SimpleModule::ModuleCtx* ctx)
{
    try {
        //user config
        first_packet_timeout
            = ctx->config().get<int>("user.first_packet_timeout"
                    , 1000 /*default*/);
        CHECK_GT(first_packet_timeout, 0);

        max_connects
            = ctx->config().get<int>("user.max_connects"
                    , 5 /*default*/);
        CHECK_GT(max_connects, 0);

        request_limit_peer_second
            = ctx->config().get<int>("user.request_limit_peer_second"
                    , 100 /*default*/);
        CHECK_GT(request_limit_peer_second, 0);

        pulse_relaxed_checking
            = ctx->config().get<bool>("user.pulse_relaxed_checking"
                    , false /*default*/);
        pulse_lazy_emitting
            = ctx->config().get<bool>("user.pulse_lazy_emitting"
                , false /*default*/);
        pulse_interval
            = ctx->config().get<int>("user.pulse_interval"
                , 1000 /*default*/);
        retry_times
            = ctx->config().get<int>("user.retry_times"
                , 3 /*default*/);
    }
    catch (const boost::property_tree::ptree_bad_data &e2) {
        LOG(ERROR) << "ptree bad data: " << e2.what();
        return false;
    }
    catch (const boost::property_tree::ptree_bad_path &e3) {
        LOG(ERROR) << "ptree bad path: " << e3.what();
        return false;
    }
    catch (const boost::property_tree::ptree_error &e1) {
        LOG(ERROR) << "ptree error: " << e1.what();
        return false;
    }
    catch (...) {
        LOG(ERROR) << "unknown exception";
        return false;
    }

    LOG(INFO) << "user first packet timeout: " << first_packet_timeout;
    LOG(INFO) << "user pulse relaxed checking: " 
        << pulse_relaxed_checking;
    LOG(INFO) << "user pulse lazy emitting: " << pulse_lazy_emitting;
    LOG(INFO) << "user pulse interval: " << pulse_interval;
    LOG(INFO) << "user max connects: " << max_connects;
    LOG(INFO) << "user reqeust limit peer second: " 
        << request_limit_peer_second;
    LOG(INFO) << "user retry times: " << retry_times;

    return true;
}

bool Server::Init(ldd::skel::SimpleModule::ModuleCtx* ctx)
{
    try {
        //server config
        request_send_timeout
            = ctx->config().get<int>("server.request_send_timeout"
                , 50 /*default*/);
        CHECK_GT(request_send_timeout, 0);

        request_recv_timeout
            = ctx->config().get<int>("server.request_recv_timeout"
                , 200 /*default*/);
        CHECK_GT(request_recv_timeout, 0);

        pulse_interval
            = ctx->config().get<int>("server.pulse_interval"
                , 100000 /*default*/);
        CHECK_GT(pulse_interval, 0);

        pulse_relaxed_checking
            = ctx->config().get<bool>("server.pulse_relaxed_checking"
                    , true /*default*/);
        pulse_lazy_emitting
            = ctx->config().get<bool>("server.pulse_lazy_emitting"
                    , true /*default*/);
    }
    catch (const boost::property_tree::ptree_bad_data &e2) {
        LOG(ERROR) << "ptree bad data: " << e2.what();
        return false;
    }
    catch (const boost::property_tree::ptree_bad_path &e3) {
        LOG(ERROR) << "ptree bad path: " << e3.what();
        return false;
    }
    catch (const boost::property_tree::ptree_error &e1) {
        LOG(ERROR) << "ptree error: " << e1.what();
        return false;
    }
    catch (...) {
        LOG(ERROR) << "unknown exception";
        return false;
    }

    LOG(INFO) << "server request send timeout: " << request_send_timeout;
    LOG(INFO) << "server request recv timeout: " << request_recv_timeout;
    LOG(INFO) << "server pulse interval: " << pulse_interval;
    LOG(INFO) << "server pulse relaxed checking: " 
        << pulse_relaxed_checking;
    LOG(INFO) << "server pulse lazy emitting: " 
        << pulse_lazy_emitting;


    return true;
}

bool Push::Init(ldd::skel::SimpleModule::ModuleCtx* ctx)
{
    try {
        send_timeout
            = ctx->config().get<int>("push.send_timeout"
                , 10 /*default*/);
        CHECK_GT(send_timeout, 0);

        recv_timeout
            = ctx->config().get<int>("push.recv_timeout"
                , 1000 /*default*/);
        CHECK_GT(recv_timeout, 0);

        once_push_users
            = ctx->config().get<int>("push.once_push_users"
                , 100/*default*/);
        CHECK_GT(once_push_users, 0);

        response_threshold 
            = ctx->config().get<int>("push.response_threshold"
                , 1000 /*default*/);
        CHECK_GT(response_threshold, 0);

        response_interval
            = ctx->config().get<int>("push.response_interval"
                , 1000 /*default*/);
        CHECK_GT(response_interval, 0);

        qlog_info
            = ctx->config().get<int>("push.qlog_info"
                , true /*default*/);
    }
    catch (const boost::property_tree::ptree_bad_data &e2) {
        LOG(ERROR) << "ptree bad data: " << e2.what();
        return false;
    }
    catch (const boost::property_tree::ptree_bad_path &e3) {
        LOG(ERROR) << "ptree bad path: " << e3.what();
        return false;
    }
    catch (const boost::property_tree::ptree_error &e1) {
        LOG(ERROR) << "ptree error: " << e1.what();
        return false;
    }
    catch (...) {
        LOG(ERROR) << "unknown exception";
        return false;
    }

    LOG(INFO) << "push message send timeout: " 
        << send_timeout;
    LOG(INFO) << "push message recv timeout: " 
        << recv_timeout;
    LOG(INFO) << "push once push users: " << once_push_users;
    LOG(INFO) << "push response threshold: " << response_threshold;
    LOG(INFO) << "push response interval: " << response_interval;


    return true;
}

bool Session::Init(ldd::skel::SimpleModule::ModuleCtx* ctx)
{
    try {
        user_check_interval 
            = ctx->config().get<int>("session.user_check_interval"
                    , 1000 /*default, ms*/);
        user_num_once_send = ctx->config().get<int>("session.user_num_once_send"
                , 100 /*default*/);

        send_timeout
            = ctx->config().get<int>("session.send_timeout"
                , 10 /*default*/);
        CHECK_GT(send_timeout, 0);

        recv_timeout
            = ctx->config().get<int>("session.recv_timeout"
                , 100/*default*/);

        CHECK_GT(recv_timeout, 0);

        max_retry_counts
            = ctx->config().get<int>("session.max_retry_counts"
                , 3/*default*/);


    }
    catch (const boost::property_tree::ptree_bad_data &e2) {
        LOG(ERROR) << "ptree bad data: " << e2.what();
        return false;
    }
    catch (const boost::property_tree::ptree_bad_path &e3) {
        LOG(ERROR) << "ptree bad path: " << e3.what();
        return false;
    }
    catch (const boost::property_tree::ptree_error &e1) {
        LOG(ERROR) << "ptree error: " << e1.what();
        return false;
    }
    catch (...) {
        LOG(ERROR) << "unknown exception";
        return false;
    }

    LOG(INFO) << "session user check interval: " 
        << user_check_interval;
    LOG(INFO) << "session user num once send: " 
        << user_num_once_send;
    LOG(INFO) << "session message send timeout: " 
        << send_timeout;
    LOG(INFO) << "session message recv timeout: " 
        << recv_timeout;

    return true;
}

bool Test::Init(ldd::skel::SimpleModule::ModuleCtx* ctx)
{
    try {
        enable = ctx->config().get<bool>("test.enable"
                    , false /*default*/);
        server = ctx->config().get<std::string>("test.server"
                , "127.0.0.1" /*default*/);
        port = ctx->config().get<int>("test.port"
                , 9527 /*default*/);
        CHECK_GT(port, 1000);
    }
    catch (const boost::property_tree::ptree_bad_data &e2) {
        LOG(ERROR) << "ptree bad data: " << e2.what();
        return false;
    }
    catch (const boost::property_tree::ptree_bad_path &e3) {
        LOG(ERROR) << "ptree bad path: " << e3.what();
        return false;
    }
    catch (const boost::property_tree::ptree_error &e1) {
        LOG(ERROR) << "ptree error: " << e1.what();
        return false;
    }
    catch (...) {
        LOG(ERROR) << "unknown exception";
        return false;
    }

    LOG(INFO) << "test enable: " 
        << (enable ? "true" : "false");
    LOG(INFO) << "test server: " 
        << server;
    LOG(INFO) << "tese server port: " 
        << port;

    return true;
}

bool Gateway::Init(ldd::skel::SimpleModule::ModuleCtx* ctx)
{
    try {
        //gateway config
        thread_count = ctx->config().get<int>("gateway.thread_count"
                , 5 /*default*/);
        CHECK_GT(thread_count, 0);

        threads_dispatching = ctx->config().get<bool>("gateway.threads_dispatching"
                , true /*default*/);

        uint64_t max = ctx->config().get<uint64_t>("gateway.max_connections"
                , 100 /*default*/);
        CHECK_GT(max, (uint64_t)0);

        max_connections = max;
        echo = ctx->config().get<bool>("gateway.echo"
                , false /*default*/);
        http_port = ctx->config().get<int>("gateway.http_port"
                , 8000 /*default*/);
        CHECK_GT(http_port, 0);


        use_login = ctx->config().get<bool>("gateway.use_login"
                , false /*default*/);

        log_conf = ctx->config().get<std::string>("gateway.log_conf"
                , "/home/s/etc/lddskel/lcs_gateway_log.conf" /*default*/);

        status_file = ctx->config().get<std::string>("gateway.status_file"
                , "/home/s/etc/lddskel/lcs_gateway_status" /*default*/);
        ip_locate_file = ctx->config().get<std::string>("gateway.ip_locate_file"
                , "/home/s/safe/lcs_gateway/etc/ip_locate.bin" /*default*/);



        lcs::gateway::log_stat_interval= ctx->config().get<int>("gateway.log_stat_interval", 1);
        lcs::gateway::log_great_than = ctx->config().get<int>("gateway.log_great_than", 100);
    }
    catch (const boost::property_tree::ptree_bad_data &e2) {
        LOG(ERROR) << "ptree bad data: " << e2.what();
        return false;
    }
    catch (const boost::property_tree::ptree_bad_path &e3) {
        LOG(ERROR) << "ptree bad path: " << e3.what();
        return false;
    }
    catch (const boost::property_tree::ptree_error &e1) {
        LOG(ERROR) << "ptree error: " << e1.what();
        return false;
    }
    catch (...) {
        LOG(ERROR) << "unknown exception";
        return false;
    }

    LOG(INFO) << "thread count: " << thread_count;
    LOG(INFO) << "max connections: " << max_connections;
    LOG(INFO) << "http port: " << http_port;
    LOG(INFO) << "echo flag : " << echo;
    LOG(INFO) << "use login: " << use_login;
    LOG(INFO) << "log conf: "  << log_conf;

    return true;
}

} /*option*/
} /*gateway*/
} /*lcs*/
