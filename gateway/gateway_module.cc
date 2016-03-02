// gateway_module_impl.cc (2013-08-07)
// yan gaofeng (yangaofeng@360.cn)

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>

#include <glog/logging.h>
//#include <gperftools/heap-profiler.h>
#include <gperftools/profiler.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/foreach.hpp>
#include <ldd/net/channel.h>
#include <ldd/net/client.h>
#include <ldd/net/server.h>
#include <ldd/net/incoming_msg.h>
#include <ldd/net/stat.h>
#include <zookeeper/zookeeper.h>

#include <qlog/qlog.h>

#include "default_msg.h"
#include "push_msg.h"
#include "gateway_module.h"

#include "ship.h"

#include "service.h"
#include "user.h"
#include "session_msg.h"



namespace lcs { namespace gateway {

using namespace std;
using namespace ldd::net;

GateWayModule::GateWayModule()
    : config_(NULL)
      , id_(0)
      , event_loop_(NULL)
      , cur_connections_(0)
      , cur_users_(0)
      , server_connections_(0)
      , total_request_(0)
      , pre_second_total_request_(0)
      , request_peer_second_(0)
      , total_pulse_timeout_(0)
      , pre_second_total_pulse_timeout_(0)
      , pulse_timeout_peer_second_(0)
      , listener_(NULL)
      , zookeeper_wait_count_(0)
      /*, heap_checker_("gateway.checker")*/
{
}

GateWayModule::~GateWayModule()
{
    http_.Stop();

    init_timer_.reset();
    stat_timer_.reset();

    typedef boost::ptr_vector<ldd::net::EventLoopThread>::auto_type auto_type;
    BOOST_FOREACH(ldd::net::EventLoopThread& thread, threads_) {
        thread.Stop();
        thread.Join();
    }

    server_.reset();
    ships_.clear();

    //HeapProfilerStop();

    //ProfilerStop();
    
    //heap_checker_.NoLeaks();
}

bool GateWayModule::InitModule(ModuleCtx* ctx)
{
    //HeapProfilerStart("/home/yangaofeng/qdev/CloudSrv/trunk/lcs/gateway/src/abc");
    //HeapProfilerStart("/home/s/var/log/gateway");

    //ProfilerStart("/home/s/var/log/gateway.prof");
    //ProfilerStart("/home/yangaofeng/qdev/CloudSrv/trunk/lcs/gateway/src");

    //char *buf = new char [10240];
    //if (buf == NULL) {
    //    return false;
    //}

    zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);

    ctx_ = ctx;

    event_loop_ = ctx->event_loop();
    listener_ = ctx->listener();

    if (!InitOption()) {
        return false;
    }

    if (!InitQlog()) {
        return false;
    }

    if (!InitShip()) {
        return false;
    }

    if (!InitZookeeper()) {
        return false;
    }

    if (!InitServer()) {
        return false;
    }

    //2,启动http server
    if (!StartHttp()) {
        return false;
    }
    if (!InitIpLocateQuery()) {
        return false;
    }

    StartStatTimer();

    return true;
}

bool GateWayModule::InitIpLocateQuery() {
    ip_locate_query_.reset(new safe::util::IpLocateQuery);
    if(!ip_locate_query_->initialize(gateway_option().ip_locate_file)) {
        LOG(ERROR) << "init ip_locate_query failed";
        return false;
    }
    return true;
}
void GateWayModule::StatInfo()
{
    //计算每秒请求数
    request_peer_second_ = total_request_ - pre_second_total_request_;
    pre_second_total_request_ = total_request_;

    //计算每秒超时数
    pulse_timeout_peer_second_ = total_pulse_timeout_ - pre_second_total_pulse_timeout_;
    pre_second_total_pulse_timeout_ = total_pulse_timeout_;

    std::ostringstream os;

    std::string lddnet_stat = ldd::net::Stat::instance().Status();
    if (!lddnet_stat.empty()) {
        os << "lddnet: ";
        os << lddnet_stat;
        os << "; ";
    }

    os << "gateway:"
        << " server_conns[" << server_connections_
        << "] user_conns[" << cur_connections_
        << "] users[" << cur_users_
        << "] req_ps[" << request_peer_second_
        << "] timeout_ps[" << pulse_timeout_peer_second_
        << "]";

    uint64_t next_id = DefaultMsg::next_id();
    uint64_t free_id = DefaultMsg::free_id();
    os << " def_msg[" <<  free_id
        << ":" << next_id
        << ":" << next_id - free_id
        << "]";

    next_id = ToServer::next_id();
    free_id = ToServer::free_id();
    os << " to_srv[" <<  free_id
        << ":" << next_id
        << ":" << next_id - free_id
        << "]";

    next_id = PushMsg::next_id();
    free_id = PushMsg::free_id();
    os << " push_msg[" <<  free_id
        << ":" << next_id
        << ":" << next_id - free_id
        << "]";

    next_id = ToClient::next_id();
    free_id = ToClient::free_id();
    os << " to_cli[" <<  free_id
        << ":" << next_id
        << ":" << next_id - free_id
        << "]";

    next_id = ToSession::next_id();
    free_id = ToSession::free_id();
    os << " to_sess[" <<  free_id
        << ":" << next_id
        << ":" << next_id - free_id
        << "]";

    LOG(WARNING) << os.str();

    //LOG(WARNING) << "Reqeust peer second: " << request_peer_second_;
    //LOG(WARNING) << "Current connections: " << cur_connections_;
    //LOG(WARNING) << "Current users      : " << cur_users_;

    stat_timer_->AsyncWait(
            boost::bind(&GateWayModule::StatInfo, this),
            ldd::util::TimeDiff::Seconds(1));
}

void GateWayModule::NodeNotifier(OP op
        , const Node &node)
{
    switch (op) {
        case kAddNode:
            AddNode(node);
            break;
        case kDelNode:
            RemoveNode(node);
            break;
        default:
            LOG(FATAL) << "unknown service notify";
            break;
    }

    return;
}

ldd::net::EventLoop *GateWayModule::event_loop()
{
    return event_loop_;
}

void GateWayModule::RemoveNode(const Node &node)
{
    DLOG(INFO) << "remove node: "
        << node.path;

    if (ships_.empty()) {
        LOG(FATAL) << "ships is empty when remove node: " 
            << node.path;
        return;
    }

    ShipIt ship_it = ships_.begin();
    for (; ship_it != ships_.end();ship_it++) {
        //放到各自的线程中去执行
        ship_it->second->event_loop()->QueueInLoop(
                boost::bind(&Ship::RemoveNode
                    , ship_it->second.get()
                    , node));
    }
}


void GateWayModule::AddNode(const Node &node)
{
    //LOG(INFO) << "adds node: " << node.path;

    if (ships_.empty()) {
        LOG(FATAL) << "ships is empty";
        return;
    }

    ShipIt ship_it = ships_.begin();
    for (; ship_it != ships_.end(); ship_it++) {
        //将添加节点操作放到各自的线程去执行
        //LOG(INFO) << "add node "
        //    << node.path
        //    << " to ship: "
        //    << ship_it->second->id();
        ship_it->second->event_loop()->QueueInLoop(
                boost::bind(&Ship::AddNode
                    , ship_it->second.get()
                    , node));
    }
}

void GateWayModule::UserConnected(const boost::shared_ptr<ldd::net::Channel> &channel)
{
    cur_connections_++;

    if (gateway_option().max_connections > 0) {
        if (cur_connections_ > gateway_option().max_connections) {
            DLOG(WARNING) << "byond max connections limit and close channel: "
                << channel->name()
                << ", cur connection: " <<  cur_connections_
                << ", max connection: " << gateway_option().max_connections;
            channel->Close();
            return;
        }
    }

    ShipIt ship_pos = ships_.find(pthread_self());
    if (ship_pos == ships_.end()) {
        LOG(FATAL) << "not found ship by thread id: " 
            << pthread_self();
        return;
    }

    if (!ship_pos->second->AddUser(channel)) {
        LOG(ERROR) << "add user failed: "
            << channel->name()
            << ", channel addr: "
            << channel.get();

        channel->Close();
    }
}

void GateWayModule::UserClosed(const boost::shared_ptr<ldd::net::Channel> &channel)
{
    cur_connections_--;
    if (cur_connections_ < 0) {
        LOG(FATAL) << "invalid current connections : " << cur_connections_;
        cur_connections_ = 0;
        return;
    }
   
    if (errno == ETIMEDOUT) {
        total_pulse_timeout_++;
    }

    if (channel->context().empty()) {
        LOG(ERROR) << "remove invalid user, port repeated: "
            << channel->name()
            << ", channel addr: "
            << channel.get();
        return;
    }

    //note, 需要判断是那个线程
    ShipIt ship_pos = ships_.find(pthread_self());
    if (ship_pos == ships_.end()) {
        LOG(FATAL) << "not found ship by thread id: " << pthread_self();
        return;
    }

    ship_pos->second->RemoveUser(channel);
}

void GateWayModule::OnZookeeperSessionExpire()
{
    LOG(ERROR) << "ZookeeperSession Expired, stopping event loop";
    event_loop_->Stop();
}

void GateWayModule::IncRequest()
{
    total_request_++;
}


bool GateWayModule::StartHttp()
{
    http_.RegisterDefaultEvent(
        std::tr1::bind(
            &GateWayModule::RequestHandler, 
            this, 
            std::tr1::placeholders::_1, 
            std::tr1::placeholders::_2, 
            std::tr1::placeholders::_3));

    http_.RegisterEvent(
                "qpoller", "status.html",
                std::tr1::bind(
                    &GateWayModule::QpollerStatus, 
                    this, 
                    std::tr1::placeholders::_1, 
                    std::tr1::placeholders::_2, 
                    std::tr1::placeholders::_3));

    http_.RegisterEvent(
                "self", "status.html",
                std::tr1::bind(
                    &GateWayModule::HtmlStatus, 
                    this, 
                    std::tr1::placeholders::_1, 
                    std::tr1::placeholders::_2, 
                    std::tr1::placeholders::_3));

    http_.RegisterEvent(
                "self", "status",
                std::tr1::bind(
                    &GateWayModule::Status, 
                    this, 
                    std::tr1::placeholders::_1, 
                    std::tr1::placeholders::_2, 
                    std::tr1::placeholders::_3));

    http_.RegisterEvent(
                "self", "services",
                std::tr1::bind(
                    &GateWayModule::Services, 
                    this, 
                    std::tr1::placeholders::_1, 
                    std::tr1::placeholders::_2, 
                    std::tr1::placeholders::_3));

    bool ret = http_.Start(gateway_option().http_port);
    if (!ret) {
        LOG(ERROR) << "start http server failed";
    }

    return ret;
}

void GateWayModule::RequestHandler(
            const mx::HTTPService::HttpContext& ctx, 
            const mx::HTTPService::ParameterMap& params, 
            mx::HTTPService::SendResponseCallback cb)
{
    DLOG(INFO) << "module=[" << ctx.module << "] method=[" << ctx.method << "]";

    if (gateway_option().status_file.empty()) {
        cb("FAILED");
        return;
    }

    std::string status;
    std::ifstream ifs(gateway_option().status_file.c_str());
    if (!ifs.good()) {
        LOG(WARNING) << "open status file failed";
        cb("FAILED");
        return;
    }

    ifs >> status;
    if (status.empty()) {
        cb("FAILED");
        return;
    }

    cb(status);
}

void GateWayModule::QpollerStatus(
            const mx::HTTPService::HttpContext& ctx, 
            const mx::HTTPService::ParameterMap& params, 
            mx::HTTPService::SendResponseCallback cb)
{
    //node need rewrite, add health check
    std::string return_string = "OK";
    //check gateway run status, such as server node count
    if (!threads_.empty()) {
        cb("OK");
    }
    else {
        cb("FAILED");
    }
}

void GateWayModule::HtmlStatus(
            const mx::HTTPService::HttpContext& ctx, 
            const mx::HTTPService::ParameterMap& params, 
            mx::HTTPService::SendResponseCallback cb)
{
    //node need rewrite, add health check
    DLOG(INFO) << "module=[" << ctx.module << "] method=[" << ctx.method << "]";

    if (!threads_.empty()) {
        cb("ok");
    }
    else {
        cb("failed");
    }
}

void GateWayModule::Status(
            const mx::HTTPService::HttpContext& ctx, 
            const mx::HTTPService::ParameterMap& params, 
            mx::HTTPService::SendResponseCallback cb)
{
    boost::property_tree::ptree pt;
    pt.put("Status.reqeust_peer_second", request_peer_second_);
    pt.put("Status.clients", cur_connections_);
    pt.put("Status.threads", gateway_option().thread_count);

    std::ostringstream js;
    boost::property_tree::json_parser::write_json(js, pt);

    cb(js.str());
    
    DLOG(INFO) << "status json: " << js.str();
}

void GateWayModule::Services(
            const mx::HTTPService::HttpContext& ctx, 
            const mx::HTTPService::ParameterMap& params, 
            mx::HTTPService::SendResponseCallback cb)
{
    std::string js = config_->ServicesJson();
    if (js.empty()) {
        LOG(WARNING) << "services json is empty";
    } 

    cb(js);
}

bool GateWayModule::InitOption()
{
    if (!user_option_.Init(ctx_)) {
        return false;
    }

    if (!server_option_.Init(ctx_)) {
        return false;
    }

    if (!push_option_.Init(ctx_)) {
        return false;
    }

    if (!session_option_.Init(ctx_)) {
        return false;
    }

    if (!zookeeper_option_.Init(ctx_)) {
        return false;
    }

    if (!test_option_.Init(ctx_)) {
        return false;
    }

    if (!gateway_option_.Init(ctx_)) {
        return false;
    }

    //get local ip
    local_ip_ = ctx_->addr().ToString();

    //make gateway id
    struct in_addr in = ctx_->addr().ToV4();
    //inet_aton(local_ip.c_str(), &in);
    id_ = in.s_addr;

    return true;
}

void GateWayModule::StartStatTimer() {
    stat_timer_.reset(new ldd::net::TimerEvent(event_loop_));
    stat_timer_->AsyncWait(
            boost::bind(&GateWayModule::StatInfo, this),
            ldd::util::TimeDiff::Seconds(1));
}


bool GateWayModule::InitServer()
{
    //2,创建Server对象
    ldd::net::Server::Options s_op;
    s_op.pulse_interval = user_option().pulse_interval;
    s_op.pulse_relaxed_checking = user_option().pulse_relaxed_checking;
    s_op.pulse_lazy_emitting = user_option().pulse_lazy_emitting;
    s_op.threads_dispatching = gateway_option().threads_dispatching ? ldd::net::Server::kHashing : ldd::net::Server::kNormal;
    s_op.notify_connected = boost::bind(&GateWayModule::UserConnected
            , this
            , _1);
    s_op.notify_closed = boost::bind(&GateWayModule::UserClosed
            , this
            , _1);

    s_op.threads = &threads_;

    server_.reset(new Server(s_op));
    //注册default消息，只接收default消息
    server_->RegisterDefaultIncomingMsg<DefaultMsg>(this);

    //6，启动server
    if (!server_->Start(listener_)) {
        LOG(ERROR) << "server start failed";
        return false;
    }

    return true;
}

void GateWayModule::ProtocolNotifier(OP op, int protocol)
{
    ldd::util::MutexLock lock(&protocol_mutex_);
    if (kAddProtocol) {
        protocols_.insert(protocol);
    } else if (kDelProtocol) {
        protocols_.erase(protocol);
    } else {
        LOG(FATAL) << "unknown protocol opreation: " << op;
        return;
    }

    return;
}

bool GateWayModule::IsValidProtocol(int protocol)
{
    ldd::util::MutexLock lock(&protocol_mutex_);
    std::set<int>::iterator pos = protocols_.find(protocol);
    if (pos == protocols_.end()) {
        return false;
    }

    return true;
}

bool GateWayModule::InitQlog()
{
    struct stat st;
    // If the config file is not exist, or is not a regular file or a symlink,
    // the path is invalid.
    if (stat(gateway_option().log_conf.c_str(), &st) < 0 
            || !(S_ISREG(st.st_mode) 
                || S_ISLNK(st.st_mode))) {
        LOG(ERROR) << "invalid qlog config file: " << gateway_option().log_conf;
        return false;
    }

    qlog::qLogConfig(gateway_option().log_conf.c_str());

    return true;
}

void GateWayModule::OnZookeeperTimer()
{
    if (!config_->IsComplete()) {
        zookeeper_wait_count_++;
        if (zookeeper_wait_count_ * 10 > zookeeper_option_.init_timeout ) {
            OnZookeeperSessionExpire();
            LOG(ERROR) << "wait for zookeeper operate greater then "
                << zookeeper_option_.init_timeout
                << "ms, reboot";
            return;
        }

        //LOG(INFO) << "waitintg for zookeeper operation complete";
        StartZookeeperTimer();
        return;
    }
    
    LOG(ERROR) << "zookeeper operate eq 0 ";
    event_loop()->Stop();
}

void GateWayModule::StartZookeeperTimer()
{
    init_timer_.reset(new ldd::net::TimerEvent(event_loop_));
    init_timer_->AsyncWait(
            boost::bind(&GateWayModule::OnZookeeperTimer, this),
            ldd::util::TimeDiff::Milliseconds(20));
}

bool GateWayModule::InitShip()
{
    for (int i = 0; i < gateway_option().thread_count; i++) {
        //创建线程
        threads_.push_back(new EventLoopThread());
        if (!threads_.back().Start()) {
            LOG(ERROR) << "event loop thread " << i << " start failed";
            return false;
        }

        //创建ship
        boost::shared_ptr<Ship> ship 
            = boost::make_shared<Ship>(this, threads_.back().event_loop());
        if (!ship) {
            LOG(FATAL) << "create ship failed";
            return false;
        }

        //获取线程id
        pthread_t id = (pthread_t)threads_.back().thread()->id();
        ship->set_id(id);

        LOG(INFO) << "create event loop thread, index: " << i
            << ", thread id: " << id;

        ships_.insert(make_pair(id, ship));
    }

    return true;
}

bool GateWayModule::InitZookeeper()
{
    //init zookeeper
    if (!test_option_.enable) {
        config_ = gateway::Config::GetInstance(zookeeper_option_
                , boost::bind(&GateWayModule::OnZookeeperSessionExpire, this)
                , local_ip_
                , ctx_->event_loop());
    }
    else {
        config_ = gateway::Config::GetInstance(test_option_);
    }

    CHECK_NOTNULL(config_);

    if (!config_->Init(
                boost::bind(&GateWayModule::NodeNotifier
                    , this, _1, _2)
            , boost::bind(&GateWayModule::ProtocolNotifier
                , this, _1, _2))) 
    {
        LOG(ERROR) << "config init failed";
        return false;
    }

    if (!config_->IsComplete()) {
        LOG(INFO) << "start zookeeper init";
        StartZookeeperTimer();

        ctx_->event_loop()->Run();


        if (!config_->IsComplete()) {
            LOG(INFO) << "zookeeper init timeout";
            return false;
        }
    }

    LOG(INFO) << "zookeeper init complete";

    return true;
}

void GateWayModule::IncUsers()
{
    cur_users_++;
}

void GateWayModule::DecUsers()
{
    cur_users_--;
}

void GateWayModule::IncServers()
{
    server_connections_++;
}

void GateWayModule::DecServers()
{
    server_connections_--;
}

} /*gateway*/
} /*lcs*/

LDD_SKEL_SIMPLE_MODULE(lcs::gateway::GateWayModule);

