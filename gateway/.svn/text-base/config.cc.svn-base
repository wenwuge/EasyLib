// config.cc (2013-07-31)
// Yan Gaofeng (yangaofeng@360.cn)

#include <glog/logging.h>
#include <zookeeper/zookeeper.h>

#include <iostream>

#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <ldd/cage/retry_policy.h>

#include "config.h"

namespace lcs {
namespace gateway {

using namespace std;
using namespace ldd::net;
using namespace ldd::cage;

const char *LCS_GATEWAY = "/gateway";
const char *LCS_SERVICES = "/services";
const char *LCS_PROTOCOLS = "/protocols";

Config *Config::instance_ = NULL;

void NotifyCage(const CageState& state
        , Config *cfg)
{
    //LOG(INFO) << "Notified: " << state.ToString();

    if (state.IsConnected()) {
        cfg->DecOp();
        cfg->WatchServices();
        cfg->WatchProtocol();
        cfg->RegisterSelf();
    }
    else if (state.IsSuspended()) {
        LOG(INFO) << "cage event connected";
    }
    else if (state.IsReconnected()) {
        LOG(INFO) << "cage event reconnected";
    }
    else if (state.IsLost()) {
        LOG(INFO) << "cage event lost";
        cfg->SessionExpired();
    }
    else if (state.IsUnknown()) {
        LOG(INFO) << "cage event lost";
        cfg->SessionExpired();
    }
}

Config::Config(const option::Zookeeper &option
        , const SessionExpireNotifier &session_expire_notifier
        , const std::string &local_host
        , ldd::net::EventLoop *event_loop)
    : init_(false)
      , option_(option)
      , local_host_(local_host)
      , event_loop_(event_loop)
      , op_(0)
      , session_expire_notifier_(session_expire_notifier)
{
    //init zookeeper
    cage_ = boost::make_shared<Cage>(event_loop_
            , boost::bind(&NotifyCage, _1, this)
            , ldd::cage::RetryPolicy::NewRetryNTimesPolicy(
                option_.retry_times
                , ldd::util::TimeDiff::Milliseconds(option_.retry_interval)));
    retry_timer_.reset(new ldd::net::TimerEvent(event_loop_));
}

Config::Config(const option::Test &option)
    : init_(false)
      , test_option_(option)
      , op_(0)
{
}

Config::~Config()
{
}

bool Config::Init(const NodeNotifier &node_notifier
        , const ProtocolNotifier &protocol_notifier)
{
    if (init_) {
        LOG(ERROR) << "can not repeat init";
        return false;
    }

    node_notifier_ = node_notifier;
    protocol_notifier_ = protocol_notifier;

    if (!test_option_.enable) {

        if (!cage_) {
            LOG(ERROR) << "invalid cage";
            return false;
        }

        IncOp();
        if (!cage_->Open(option_.host, option_.timeout)) {
            LOG(ERROR) << "cage init failed";
            return false;
        }
    }
    else {
        //construct server info
        ConstructTestInfo();
    }

    init_ = true;

    return true;
}

bool Config::JsonToNode(const std::string &json, Node *node)
{
    CHECK_NOTNULL(node);

    if (json.empty()) {
        LOG(ERROR) << "json string can not be empty";
        return false;
    }

    DLOG(INFO) << "node " << node->path << " json value: " << json;

    boost::property_tree::ptree pt;
    std::istringstream iss(json);
    try {
        boost::property_tree::json_parser::read_json(iss, pt);

        //get host
        node->host = pt.get<std::string>("host");
        if (node->host.empty()) {
            LOG(ERROR) << "get " 
                << json << " host error: host string is empty";
            return false;
        }

        //get startup_time
        node->startup_time = pt.get<std::string>("startup_time");
        if (node->startup_time.empty()) {
            LOG(ERROR) << "get " << json << " startup_time error";
            return false;
        }
        
        try {
            //replace ip
            std::string ip = pt.get<std::string>("ip");
            if (!node->ip.empty()) {
                node->ip = ip;
            }
        }
        catch(...) {
        }
 
        //get ports
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
                            pt.get_child("port"))
        {
            try {
                int port = boost::lexical_cast<int>(v.second.data());
                node->port.push_back(port);
            }
            catch (boost::bad_lexical_cast &e) {
                LOG(ERROR) << "convert string port to "
                    "int port value exception: " << e.what();
                return false;
            }
        }

        //get protocols
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
                            pt.get_child("protocol"))
        {
            try {
                int port = boost::lexical_cast<int>(v.second.data());
                node->protocol.push_back(port);
            }
            catch (boost::bad_lexical_cast &e) {
                LOG(ERROR) << "convert string protocol to "
                    "int protocol value exception: " << e.what();
                return false;
            }
        }
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

    if (node->port.empty()) {
        LOG(ERROR) << "get " << json
            << " ports failed";
        return false;
    }

    return true;
}

Config *Config::GetInstance(const option::Zookeeper &option
        , const SessionExpireNotifier &session_expire_notifier
        , const std::string &local_host
        , ldd::net::EventLoop *event_loop)
{
    if (instance_ == NULL) {
        instance_ = new Config(option
                , session_expire_notifier
                , local_host
                , event_loop);
    }

    return instance_;
}

Config *Config::GetInstance(const option::Test &option)
{
    if (instance_ == NULL) {
        instance_ = new Config(option);
    }

    return instance_;
}

int Config::ConvertProtocolToInt(const std::string &protocol)
{
    int protocol_intval = 0;
    try {
        protocol_intval = boost::lexical_cast<int>(protocol);
    }
    catch (boost::bad_lexical_cast &e) {
        LOG(ERROR) << "convert string protocol to "
            "int proto value exception: " << e.what();
        return -1;
    }

    return protocol_intval;
}

//service such as /lcs/zwt/services
void Config::WatchServices()
{
    string path = option_.root_path + LCS_SERVICES;
    LOG(INFO) << "watch services: "
        << path;

    IncOp();
    cage_->GetChildren(path
            , boost::bind(&Config::ServicesWatcher
                , this
                , _1)
            , boost::bind(&Config::OnServices
                , this
                , path
                , _1));
}

void Config::ServicesWatcher(const ChildEvent& child_event)
{
    LOG(INFO) << "service watcher: " << child_event.path();

    //重新安装监视点
    WatchServices();
}

void Config::OnServices(const std::string &path
        , const GetChildrenResult& result)
{
    DecOp();

    if (!result.status().IsOk()) {
        LOG(ERROR) << "watch "
            << path
            << " error: "
            << result.status().ToString();

        if (result.status().IsNoAuth()
                || result.status().IsInvalidState()) {
            SessionExpired();
        }

        return;
    }

    CheckServices(result.children());
}

//path such as /lcs/services/bjdt/kill
void Config::WatchServicesChild(const std::string &name)
{
    std::string path = option_.root_path + LCS_SERVICES + "/" + name;

    DLOG(INFO) << "watch services child: " << path;

    IncOp();
    cage_->GetChildren(path
            , boost::bind(&Config::ServicesChildWatcher
                , this
                , name
                , _1)
            , boost::bind(&Config::OnServicesChild
                , this
                , name
                , _1));
}

void Config::ServicesChildWatcher(const std::string &name
        , const ChildEvent& child_event)
{
    DLOG(INFO) << "services child watcher weakup: " << child_event.path();

    WatchServicesChild(name);
}

void Config::OnServicesChild(const std::string &name
        , const GetChildrenResult& result)
{
    DecOp();

    if (!result.status().IsOk()) {
        LOG(ERROR) << "services child "
            << name
            << " status: "
            << result.status().ToString();

        if (result.status().IsNoAuth()
                || result.status().IsInvalidState()) {
            SessionExpired();
        }

        return;
    }

    CheckServiceNode(name, result.children());
}

/*
 * 在/lcs/idc/gateway下注册自己
 */
void Config::RegisterSelf()
{
    //SelfExists();
    //DeleteSelf();
    CreateSelf();
}

void Config::SelfExists()
{
    std::string path = option_.root_path + LCS_GATEWAY + "/" + local_host_;
    IncOp();
    cage_->Exists(path
            , boost::bind(&Config::SelfExistsWatcher, this, _1)
            , boost::bind(&Config::OnSelfExists, this, _1));
}

void Config::SelfExistsWatcher(const NodeEvent &node_event)
{
    LOG(INFO) << "self exists watcher weakup: " << node_event.path();
}

void Config::OnSelfExists(const ExistsResult &result)
{
    DecOp();

    std::string path = option_.root_path + LCS_GATEWAY + "/" + local_host_;
    if (!result.status().IsOk()) {
        //create self
        LOG(INFO) << "self " << path << " not exists and create";
        CreateSelf();
    }
    else {
        //delete self
        LOG(WARNING) << "self" << path << " has existed and delete it";
        DeleteSelf();
    }
}

void Config::DeleteSelf()
{
    std::string path = option_.root_path + LCS_GATEWAY + "/" + local_host_;
    LOG(INFO) << "delete self: " << path;
    IncOp();
    cage_->Delete(path, -1
            , boost::bind(&Config::OnDeleteSelf, this, _1));
}

void Config::OnDeleteSelf(const DeleteResult &result)
{
    DecOp();
    if (!result.status().IsOk()) {
        LOG(ERROR) << "delete self failed: " 
            << result.status().ToString();
    }

    //create self
    LOG(INFO) << "delete self success and create self";
    CreateSelf();
}

void Config::CreateSelf()
{
    std::string path = option_.root_path + LCS_GATEWAY + "/" + local_host_;

    LOG(INFO) << "create self: " << path; 

    vector<ldd::cage::Acl> acls;
    acls.push_back(ldd::cage::Acl::UnsafeAll());
    IncOp();
    cage_->Create(path
            , ""
            , acls
            , ldd::cage::Mode::kEphemeral
            , boost::bind(&Config::OnCreated
                , this
                , _1));
}

void Config::ReCreateSelf()
{
    LOG(INFO) << "recreate self";
    DecOp();
    CreateSelf();
}

void Config::StartRetryTimer()
{
    LOG(INFO) << "retry create self";
    IncOp();
    retry_timer_->AsyncWait(boost::bind(&Config::ReCreateSelf, this)
            , ldd::util::TimeDiff::Milliseconds(option_.retry_interval));
}

void Config::OnCreated(const ldd::cage::CreateResult& result)
{
    DecOp();

    if (!result.status().IsOk()) {
        if (result.status().IsNodeExists()) {
            LOG(INFO) << "register self failed, node is exists, "
                << "sleep " 
                << option_.retry_interval
                << "ms and retry";
            StartRetryTimer();
        }
        else {
            LOG(ERROR) << "register self failed: " << result.status().ToString();
        }
    }
    else {
        LOG(INFO) << "register self ok";
    }
}

void Config::GetNode(const std::string &service_name
        , const std::string &node_name)
{
    std::string path 
        = option_.root_path + LCS_SERVICES + "/" + service_name + "/" + node_name;

    DLOG(INFO) << "get node: " << path;

    IncOp();
    cage_->Get(path
            , boost::bind(&Config::NodeWatcher
                , this
                , _1)
            , boost::bind(&Config::OnNode
                , this
                , service_name
                , node_name
                , _1));
}

void Config::NodeWatcher(const NodeEvent &node_event)
{
    DLOG(INFO) << "node watcher weakup: " << node_event.path();

    if (node_event.IsDeleted()) {
        //删除节点
        DelNode(node_event.path());
    }
    else if (node_event.IsCreated()) {
        LOG(INFO) << "create " << node_event.path();
    }
    else if (node_event.IsChanged()) {
        LOG(INFO) << "change " << node_event.path();
    }
}

void Config::OnNode(const std::string &service_name
        , const std::string &node_name
        , const GetResult &result)
{
    DecOp();

    std::string path 
        = option_.root_path + LCS_SERVICES + "/" + service_name + "/" + node_name;

    if (!result.status().IsOk() ) {
        LOG(ERROR) << "get node "
            << path
            << " error: "
            << result.status().ToString();

        if (result.status().IsNoAuth()
                || result.status().IsInvalidState()) {
            SessionExpired();
        }

        return;
    }

    Node node(path, node_name, service_name);

    if (!JsonToNode(result.data(), &node)) {
        LOG(ERROR) << "convert json to node error, json value: "
            << result.data();
        return;
    }

    //添加节点
    AddNode(node);
}


void Config::WatchProtocol()
{
    string path = option_.root_path + LCS_PROTOCOLS;

    LOG(INFO) << "watch protocol: " << path;

    IncOp();
    cage_->GetChildren(path
            , boost::bind(&Config::ProtocolWatcher
                , this
                , _1)
            , boost::bind(&Config::OnProtocol
                , this
                , path
                , _1));

    return;
}

void Config::ProtocolWatcher(const ChildEvent& child_event)
{
    LOG(INFO) << "protocol watcher weakup: " << child_event.path();
    WatchProtocol();
}


void Config::OnProtocol(const std::string &path
        , const GetChildrenResult& result)
{
    DecOp();

    if (!result.status().IsOk()) {
        LOG(ERROR) << "get protocol: "
            << path
            << " error: "
            << result.status().ToString();

        if (result.status().IsNoAuth()
                || result.status().IsInvalidState()) {
            SessionExpired();
        }

        return;
    }

    //遍历返回结果，监视login,sconf等
    CheckProtocol(result.children());
}

void Config::GetProtocolValue(const std::string &name)
{
    std::string path = option_.root_path + LCS_PROTOCOLS + "/" + name;

    LOG(INFO) << "get protocol value: " << path;

    IncOp();
    cage_->Get(path
            , boost::bind(&Config::ProtocolValueWatcher
                , this
                , name
                , _1)
            , boost::bind(&Config::OnProtocolValue
                , this
                , name
                , _1));
}

void Config::ProtocolValueWatcher(const std::string &name
        , const NodeEvent &node_event)
{
    LOG(INFO) << "protocol value watcher weakup: " << node_event.path();

    if (node_event.IsDeleted()) {
        //删除节点
        DelProtocol(name);
    }
    else if (node_event.IsCreated()) {
        LOG(INFO) << "create " << node_event.path();
    }
    else if (node_event.IsChanged()) {
        LOG(INFO) << "change " << node_event.path();
    }
}

void Config::OnProtocolValue(const std::string &name
        , const GetResult &result)
{
    LOG(INFO) << "protocol "
        << name
        << ": "
        << result.data();

    DecOp();
    AddProtocol(name);
}

void Config::SessionExpired()
{
    LOG(INFO) << "zookeeper session expired";

    if (session_expire_notifier_) {
        session_expire_notifier_();
        return;
    }
    else {
        LOG(FATAL) << "invalid session expired notifier";
    }
}

void Config::AddNode(const Node &node)
{
    DLOG(INFO) << "add node: " << node.path;
    if (node_notifier_) {
        node_notifier_(kAddNode, node);
    }
    else {
        LOG(FATAL) << "invalid node notifier";
    }
}

void Config::DelNode(const std::string &node_path)
{
    DLOG(INFO) << "del node: " << node_path;
    Node node(node_path, "", "");
    if (node_notifier_) {
        node_notifier_(kDelNode, node);
    }
    else {
        LOG(FATAL) << "invalid node notifier when delete node";
    }
}

void Config::DelProtocol(const std::string &name)
{
    LOG(INFO) << "delete protocol: " << name;
    
    int protocol_val = ConvertProtocolToInt(name);
    if (protocol_val == -1) {
        LOG(ERROR) << "can not convert protocol string to int: "
            << name;
        return;
    }

    if (protocol_notifier_) {
        protocol_notifier_(kDelProtocol, protocol_val);
    }
    else {
        LOG(FATAL) << "invalid protocol notifier";
    }
}

void Config::AddProtocol(const std::string &name)
{
    LOG(INFO) << "add protocol: " << name;

    int protocol_val = ConvertProtocolToInt(name);
    if (protocol_val == -1) {
        LOG(ERROR) << "can not convert protocol string to int: "
            << name;
        return;
    }

    if (protocol_notifier_) {
        protocol_notifier_(kAddProtocol, protocol_val);
    }
    else {
        LOG(FATAL) << "invalid protocol notifier";
    }
}

void Config::IncOp()
{
    op_++;
}

void Config::DecOp()
{
    op_--;
}

//such as /lcs/bjdt/services/push
void Config::CheckServices(const std::vector<std::string> &services)
{
    //1,找出删除的
    std::vector<std::string>::iterator old_it;
    std::vector<std::string>::const_iterator new_it;

#if 0
    DLOG(INFO) << "find delete service";
    for (old_it = services_.begin(); old_it != services_.end(); old_it++) {
        for (new_it = services.begin(); new_it != services.end(); new_it++) {
            if (*old_it == *new_it) {
                break;
            }
        }

        //没找到，表示删除
        if (new_it == services.end()) {
            LOG(INFO) << "services " << *old_it << " is removed";
            //只输出删除了那个service，不需要调用RemoveService
            //zookeeper保证节点被删除时，子节点必须为空
            //RemoveService(*old_it);
        }
    }
#endif
    //找出添加的
    DLOG(INFO) << "find add service";
    for (new_it = services.begin(); new_it != services.end(); new_it++) {
        old_it = std::find(services_.begin(), services_.end(), *new_it);
        //没找到，表示添加
        if (old_it == services_.end()) {
            //为服务项添加监视点, such as kill
            WatchServicesChild(*new_it);
        }
    }

    //set old service list to new service list, include add, remove etc.
    services_ = services;
}


void Config::CheckServiceNode(const std::string &service_name
        , const std::vector<std::string> &nodes)
{
    //没有可用节点，传递本地节点列表,删除所有的node
    NodeIt pos = service_nodes_.find(service_name);
    if (pos == service_nodes_.end()) {
        if (nodes.empty()) {
            LOG(WARNING) << "check "
                << service_name
                << " node, nodes is empty, "
                ", nothing to do";
            return;
        }

        DLOG(INFO) << "add service: " 
            << service_name
            << ", node count: "
            << nodes.size();

        std::vector<std::string>::const_iterator it;
        for (it = nodes.begin(); it != nodes.end(); it++) {
            DLOG(INFO) << "first get " << service_name << " node: " << *it;
            GetNode(service_name, *it);
        }

        //add node to nodes
        std::pair<std::map<std::string, std::vector<std::string> >::iterator, bool> 
            res = service_nodes_.insert(std::make_pair(service_name, nodes));
        if (!res.second) {
            LOG(ERROR) << "insert node to nodes error";
            return;
        }

        return;
    }

    //找出删除的
    std::vector<std::string>::iterator node_it;
    std::vector<std::string>::const_iterator str_it;

    for (node_it = pos->second.begin()
            ; node_it != pos->second.end()
            ; )
    {
        str_it = std::find(nodes.begin(), nodes.end(), *node_it);
        if (str_it == nodes.end()) {
            std::string node_path 
                = option_.root_path + LCS_SERVICES + "/" + service_name + "/" + *node_it;

            DLOG(INFO) << "remove service " 
                << service_name 
                << " node: " 
                << *node_it;

            DelNode(node_path);
            node_it = pos->second.erase(node_it);
        }
        else {
            node_it++;
        }
    }


    //找出添加的
    for (str_it = nodes.begin()
            ; str_it != nodes.end()
            ; str_it++) {
        node_it = std::find(pos->second.begin(), pos->second.end(), *str_it);
        if (node_it == pos->second.end()) {
            DLOG(INFO) << "service " 
                << service_name 
                << " add new node: " 
                << *str_it;
            //添加节点，异步获取节点的json数据
            pos->second.push_back(*str_it);
            GetNode(service_name, *str_it);
        }
    }
}

void Config::CheckProtocol(const std::vector<std::string> &protocols)
{
    //找出删除的
    LOG(INFO) << "find delete protocol";
    std::vector<std::string>::iterator old_it;
    std::vector<std::string>::const_iterator new_it;
    for (old_it = protocols_.begin(); old_it != protocols_.end(); old_it++) {
        new_it = std::find(protocols.begin(), protocols.end(), *old_it);
        //没找到，表示删除
        if (new_it == protocols.end()) {
            LOG(INFO) << "remove protocol " << *old_it;
            //为第一个protocol节点添加了监视点，这里不需要再处理更多
        }
    }

    //找出添加的
    LOG(INFO) << "find add protocol";
    for (new_it = protocols.begin(); new_it != protocols.end(); new_it++) {
        old_it = std::find(protocols_.begin(), protocols_.end(), *new_it);
        //没找到，表示添加
        if (old_it == protocols_.end()) {
            LOG(INFO) << "add protocol: " << *new_it;
            GetProtocolValue(*new_it);
        }
    }

    protocols_ = protocols;
}

void Config::ConstructTestInfo()
{
    //construct test node
    LOG(INFO) << "construct test info, server node: " 
        << test_option_.server
        << ":"
        << test_option_.port;

    Node node("/root", test_option_.server, "");
    node.host = test_option_.server;
    node.ip = test_option_.server;
    node.port.push_back(test_option_.port);
    node.protocol.push_back(7);
#if 0
    node.protocol.push_back(100);
    node.protocol.push_back(1000);
    node.protocol.push_back(1001);
    node.protocol.push_back(1099);
    node.protocol.push_back(1100);
    node.protocol.push_back(1101);
    node.protocol.push_back(1200);
#endif

    AddNode(node);

    if (protocol_notifier_) {
        protocol_notifier_(kAddProtocol, 7);
        protocol_notifier_(kAddProtocol, 100);
        protocol_notifier_(kAddProtocol, 1000);
        protocol_notifier_(kAddProtocol, 1001);
        protocol_notifier_(kAddProtocol, 1099);
        protocol_notifier_(kAddProtocol, 1100);
        protocol_notifier_(kAddProtocol, 1101);
        protocol_notifier_(kAddProtocol, 1200);
    }
    else {
        LOG(FATAL) << "invalid protocol notifier";
    }
}

std::string Config::ServicesJson()
{
    boost::property_tree::ptree pt;

    std::map<std::string, std::vector<std::string> >::iterator mit;

    for (mit = service_nodes_.begin(); mit != service_nodes_.end(); mit++) {

        LOG(WARNING) << "service name: " << mit->first;

        std::string s = "services." + mit->first + ".name";

        pt.put(s, mit->first);

        std::vector<std::string>::iterator vit;
        for (vit = mit->second.begin(); vit != mit->second.end(); vit++) {

            LOG(WARNING) << "service node: " << *vit;
            std::string n = "services." + mit->first + ".nodes.node";

            pt.add(n, *vit);
        }
    }

    std::ostringstream js;
    boost::property_tree::json_parser::write_json(js, pt);
    
    return js.str();
}

} /*gateway*/ 
} /*lcs*/

