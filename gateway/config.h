// config_impl.h (2013-08-08)
// Yan Gaofeng (yangaofeng@360.cn)
#ifndef __ZK_CONFIG_IMPL_H__
#define __ZK_CONFIG_IMPL_H__

#include <vector>
#include <map>
#include <list>

#include "ldd/net/event_loop.h"
#include "ldd/cage/cage.h"
#include "options.h"

using namespace ldd::cage;

namespace lcs {
namespace gateway {

enum OP {
    kAddNode,
    kDelNode,
    kAddProtocol,
    kDelProtocol
};

struct Node {
    Node(const std::string &node_path
            , const std::string &node_name
            , const std::string &service_name) 
        : path(node_path)
    , ip(node_name)
    , service(service_name) {}

    std::string path;
    std::string ip;
    std::string service;
    std::string host;
    std::vector<int> port;
    std::vector<int> protocol;
    std::string startup_time;
};

typedef boost::function<void()> SessionExpireNotifier;

class Config {
public:
    Config(const option::Zookeeper &option
            , const SessionExpireNotifier &session_expire_notifier
            , const std::string &local_host
            , ldd::net::EventLoop *event_loop);

    Config(const option::Test &option);
    ~Config();

    typedef boost::function<void(OP op, const Node &)> NodeNotifier;
    typedef boost::function<void(OP op, int protocol)> ProtocolNotifier;

    typedef std::map<std::string, std::vector<std::string> >::iterator NodeIt;

    bool Init(const NodeNotifier &node_notifier
            , const ProtocolNotifier &protocol_notifier);
    bool IsComplete() { return op_ == 0; }

    static Config *GetInstance(const option::Zookeeper &option
        , const SessionExpireNotifier &session_expire_notifier
        , const std::string &local_host
        , ldd::net::EventLoop *event_loop);

    static Config *GetInstance(const option::Test &option);

    std::string ServicesJson();

private:
    bool JsonToNode(const std::string &json, Node *node);
    int ConvertProtocolToInt(const std::string &protocol);

    void IncOp();
    void DecOp();

    void WatchServices();
    void WatchProtocol();
    void RegisterSelf();

    void CheckServices(const std::vector<std::string> &services);
    void CheckProtocol(const std::vector<std::string> &protocols);
    void CheckServiceNode(const std::string &service_name
            , const std::vector<std::string> &nodes);

    void ServicesWatcher(const ChildEvent& child_event);
    void OnServices(const std::string &path
            , const GetChildrenResult& result);

    void WatchServicesChild(const std::string &path);
    void ServicesChildWatcher(const std::string &name
            , const ChildEvent& child_event);
    void OnServicesChild(const std::string &name
            , const GetChildrenResult& result);
    void SelfExists();
    void SelfExistsWatcher(const NodeEvent &node_event);
    void OnSelfExists(const ExistsResult &result);
    void DeleteSelf();
    void OnDeleteSelf(const DeleteResult &result);
    void CreateSelf();
    void ReCreateSelf();
    void StartRetryTimer();
    void OnCreated(const ldd::cage::CreateResult& result);
    void GetNode(const std::string &path, const std::string &name);
    void NodeWatcher(const NodeEvent &node_event);
    void OnNode(const std::string &service_name
            , const std::string &node_name
            , const GetResult &result);
    void ProtocolWatcher(const ChildEvent& child_event);
    void OnProtocol(const std::string &path
            , const GetChildrenResult& result);
    void GetProtocolValue(const std::string &name);
    void ProtocolValueWatcher(const std::string &name
            , const NodeEvent &node_event);
    void OnProtocolValue(const std::string &name
            , const GetResult &result);

    void SessionExpired();

    void AddNode(const Node &node);

    void DelNode(const std::string &node_path);

    void DelProtocol(const std::string &name);
    void AddProtocol(const std::string &name);

    void ConstructTestInfo();

    friend void NotifyCage(const CageState& state
        , Config *cfg);

private:
    bool init_;
    option::Zookeeper option_;
    option::Test test_option_;
    std::string local_host_;
    ldd::net::EventLoop *event_loop_;

    boost::shared_ptr<ldd::cage::Cage> cage_;
    int op_;

    std::vector<std::string> services_;
    std::map<std::string, std::vector<std::string> > service_nodes_;
    std::vector<std::string> protocols_;

    NodeNotifier node_notifier_;
    ProtocolNotifier protocol_notifier_;

    SessionExpireNotifier session_expire_notifier_;

    static Config *instance_;
    boost::scoped_ptr<ldd::net::TimerEvent> retry_timer_;
};

} /*gateway*/
} /*lcs*/

#endif /*__ZK_CONFIG_IMPL_H__*/

