// gateway_module_impl.h (2013-08-09)
// Yan Gaofeng (yangaofeng@360.cn)

#ifndef __GATEWAY_MODULE_IMPL_H__
#define __GATEWAY_MODULE_IMPL_H__

#include <pthread.h>
#include <map>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <ldd/net/channel.h>
#include <ldd/util/mutex.h>
#include <ldd/skel/module.h>
#include <ldd/net/event.h>
#include <ldd/net/server.h>


#include "safeutil/include/ip_locate_query.h"

#include <http/include/standalone_http_server.h>

//#include <gperftools/heap-checker.h>

#include "config.h"
#include "ship.h"

namespace lcs { namespace gateway {

class User;
class Service;
class Ship;

class GateWayModule: public ldd::skel::SimpleModule {
public:
    GateWayModule();
    ~GateWayModule();

    virtual bool InitModule(ModuleCtx* ctx);

    //用来向客户端推送消息
    boost::shared_ptr<User> GetUser(uint32_t ip, uint16_t port);
    void RemoveUser(const boost::shared_ptr<Ship> &ship, uint32_t ip, uint16_t port);

    //用来向后台服务器发送请求消息
    boost::shared_ptr<ldd::net::Channel> GetServer(int msg_type);

    bool IsValidProtocol(int protocol);

    uint32_t id() { return id_; }

    ldd::net::EventLoop *event_loop();

    //uint64_t current_users() { return cur_connections_; }

    const option::User &user_option() const { return user_option_; }
    const option::Server &server_option() const { return server_option_; }
    const option::Push &push_option() const { return push_option_; }
    const option::Session &session_option() const { return session_option_; }
    const option::Gateway &gateway_option() const { return gateway_option_; }
    const option::Zookeeper &zookeeper_option() const { return zookeeper_option_; }
    const option::Test &test_option() const { return test_option_; }
    const std::tr1::shared_ptr<safe::util::IpLocateQuery>& ip_locate_query() const { return ip_locate_query_;}

    void UserConnected(const boost::shared_ptr<ldd::net::Channel> &channel);
    void UserClosed(const boost::shared_ptr<ldd::net::Channel> &channel);

    void IncRequest();

    void IncUsers();
    void DecUsers();
    void IncServers();
    void DecServers();


    //debug function
    void PrintService(const char *func);

    void StatInfo();

    void OnZookeeperSessionExpire();
    void ClientNotifier(boost::shared_ptr<ldd::net::Channel> channel);
    //void ServiceNotifier(OP op, const std::string &service_name
    //        , const std::list<ServiceNode> &service_nodes);
    void NodeNotifier(OP op
            , const Node &node);

    void ProtocolNotifier(OP op, int protocol);

    bool InitShip();
    bool InitOption();
    void StartStatTimer();
    bool InitServer();
    bool InitQlog();
    bool InitIpLocateQuery();


    bool InitZookeeper();
    void StartZookeeperTimer();
    void OnZookeeperTimer();

    void RemoveNode(const Node &node);
    void AddNode(const Node &node);

    bool StartHttp();

    void QpollerStatus(
                const mx::HTTPService::HttpContext& ctx, 
                const mx::HTTPService::ParameterMap& params, 
                mx::HTTPService::SendResponseCallback cb);

    void HtmlStatus(
                const mx::HTTPService::HttpContext& ctx, 
                const mx::HTTPService::ParameterMap& params, 
                mx::HTTPService::SendResponseCallback cb);

    void Status(
                const mx::HTTPService::HttpContext& ctx, 
                const mx::HTTPService::ParameterMap& params, 
                mx::HTTPService::SendResponseCallback cb);

    void Services(
                const mx::HTTPService::HttpContext& ctx, 
                const mx::HTTPService::ParameterMap& params, 
                mx::HTTPService::SendResponseCallback cb);

    void RequestHandler(
                const mx::HTTPService::HttpContext& ctx, 
                const mx::HTTPService::ParameterMap& params, 
                mx::HTTPService::SendResponseCallback cb);
private:
    friend class Ship;
    friend class User;
    typedef std::map<pthread_t,  boost::shared_ptr<Ship> >::iterator ShipIt;

private:
    lcs::gateway::Config *config_;
    boost::scoped_ptr<ldd::net::Server> server_;

    std::map<pthread_t,  boost::shared_ptr<Ship> > ships_;

    //protocol
    ldd::util::Mutex protocol_mutex_;
    std::set<int> protocols_;

    uint32_t id_; //见common.proto文件，以网络字节序的ip地址来生成

    ldd::net::EventLoop *event_loop_; //主线程

    //options
    option::User user_option_;
    option::Server server_option_;
    option::Push push_option_;
    option::Session session_option_;
    option::Gateway gateway_option_;
    option::Zookeeper zookeeper_option_;
    option::Test test_option_;

    ldd::util::Atomic<uint64_t> cur_connections_;
    ldd::util::Atomic<uint64_t> cur_users_;
    ldd::util::Atomic<uint64_t> server_connections_;

    boost::ptr_vector<ldd::net::EventLoopThread> threads_;
    boost::scoped_ptr<ldd::net::TimerEvent> stat_timer_;

    //status
    ldd::util::Atomic<uint64_t> total_request_;
    uint64_t pre_second_total_request_;
    uint64_t request_peer_second_;

    ldd::util::Atomic<uint64_t> total_pulse_timeout_;
    uint64_t pre_second_total_pulse_timeout_;
    uint64_t pulse_timeout_peer_second_;


    //http server
    mx::StandaloneHTTPServer http_;

    ModuleCtx* ctx_;

    boost::scoped_ptr<ldd::net::TimerEvent> init_timer_;

    ldd::net::Listener* listener_;
    std::string local_ip_ ;
    std::tr1::shared_ptr<safe::util::IpLocateQuery>  ip_locate_query_;

    int zookeeper_wait_count_;

    //HeapLeakChecker heap_checker_;
};

} /*gateway*/
} /*lcs*/


#endif /*__GATEWAY_MODULE_IMPL_H__*/

