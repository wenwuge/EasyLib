// proxy.cc (2013-07-11)
// Yan Gaofeng (yangaofeng@360.cn)

#include <glog/logging.h>

#include <vector>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/functional/hash.hpp>

#include "c2p/get.h"
#include "c2p/list.h"
#include "c2p/check.h"
#include "c2p/mutate.h"
#include "c2p/atomic_incr.h"
#include "c2p/atomic_append.h"

#include "proxy.h"

using namespace ldd::net;
using namespace ldd::courier;


Proxy::Proxy(ldd::net::EventLoop* event_loop, 
             ldd::net::Client::Options& client_opt,
             const std::string& home_host_port,
             const std::string& zookeeper_host,
             const std::string& zookeeper_rootpath,
             int zookeeper_timeout,
             int recv_timeout, 
             int done_timeout,
             int role_type,
             bool proxy_stat) 

             : event_loop_(event_loop), option_(client_opt),                    
                home_host_port_(home_host_port),
                zookeeper_host_(zookeeper_host),
                zookeeper_rootpath_(zookeeper_rootpath), 
                zookeeper_timeout_(zookeeper_timeout),
                recv_timeout_(recv_timeout), 
                done_timeout_(done_timeout),
                role_type_(role_type),
                proxy_stat_(proxy_stat){

    client_ = NULL;
}

Proxy::~Proxy(){
    LOG(INFO)<<"Proxy::~Proxy()";
    if (proxy_stat_){
        std::map<std::string, int64_t>::const_iterator itr=read_stat_.begin();
        for (; itr!=read_stat_.end(); itr++){
            LOG(ERROR)<<"read node address:"<<itr->first
                <<" request number="<<itr->second;
        }

        std::map<std::string, int64_t>::const_iterator itr_write = write_stat_.begin();
        for (; itr_write!=write_stat_.end(); itr_write++){
            LOG(ERROR)<<"write node address:"<<itr_write->first
                <<" request number="<<itr_write->second;
        }
    }

    delete  client_;
}

void Proxy::NodeChangeCallBack(int ret, ldd::courier::NodeChangedType type, 
                               const std::string& node){
    if (type == ldd::courier::kCreateNode) {
        LOG(INFO) << "kCreateNode, node: " << node;
        ConnectNode(node);
    }
    else if (type == ldd::courier::kDeleteNode) {
        LOG(INFO) << "kDeleteNode, node: " << node;
        RemoveNode(node);
    }
}


int Proxy::GetNameSpaceID(const std::string &ns){
    int id;
    int ret = configure_->GetNameSpaceID(ns, &id);
    if (ret != 0) {
        LOG(ERROR) << "get name space id error: " << ret;
        return -1;
    }

    return id;
}

std::size_t Proxy::GetHashValue(const ldd::util::Slice &key){
    std::size_t hash_val = boost::hash_range(key.data(), key.data()+key.size()); 
    DLOG(INFO)<<"Proxy::GetHashValue() key.size()="<<key.size()
            <<" hash_val="<<hash_val<<" key="<<key.data();
    return hash_val;
}

int Proxy::GetBucketID(std::size_t hash_value){
    //calculate bucket id
    //bucket size 应该是固定的，这个可以在init初始化
    int bucket_size = 0;
    int ret = configure_->GetBucketSize(&bucket_size);
    if (ret != 0) {
        LOG(ERROR) << "get bucket size error: " << ret;
        return -1;
    }

    CHECK(bucket_size != 0) << "invalid bucket_size";
    int bucket_id = hash_value % bucket_size;
    DLOG(INFO) << "bucket size: " << bucket_size<<" hash_value="
            <<hash_value<<" bucket_id="<<bucket_id;

    return bucket_id;
}

int Proxy::GetFarmID(int bucket_id){
    int farm_id = 0;
    int ret = configure_->FindFarmIdInBucketList(bucket_id, &farm_id);
    if (ret != 0) {
        LOG(ERROR) << "Proxy::GetFarmID() get form id error: " << ret;
        return -1;
    }

    DLOG(INFO) << "bucket_id=" << bucket_id<<" farm_id="<<farm_id;
    return farm_id;
}

boost::shared_ptr<Channel> Proxy::GetReadChannel(std::size_t hash_value, 
                                                 int farm_id){
    hash_value = hash_value>>(sizeof(std::size_t)<<2);
    boost::shared_ptr<Channel> null_channel;
    //1,get node name
    Roles* roles = NULL;
    int ret = configure_->GetRoles(farm_id, &roles);
    if (ret != 0) {
        LOG(ERROR) << "get read roles error";
        return null_channel;
    }

    if (roles->slaves.size() < 0) {
        //只从slave读
        return null_channel;
    }

    std::string node_name;
    if (role_type_ == 1){
        node_name = roles->master;
        DLOG(INFO)<<"Proxy::GetReadChannel() select master="<<node_name;
    }else if(role_type_ == 2 && roles->slaves.size() > 0){
        node_name = roles->slaves[0];  // todo this only right for 1 master----1 slave
        DLOG(INFO)<<"Proxy::GetReadChannel() select slave="<<node_name;
    }else{
        int index = hash_value % (roles->slaves.size() + 1); //+1 = master     // todo bug, ony select slave;
        if (index == 0) {
            node_name = roles->master;
            /*if (farm_id == 0 || farm_id == 1){
                DLOG(ERROR)<<"Proxy::GetReadChannel() get master node_name="<<node_name
                    <<" hash_value"<<hash_value<<" index="<<index;
            }*/
        }
        else {
            node_name = roles->slaves[index - 1];
            DLOG(INFO)<<"Proxy::GetReadChannel() get slave node_name="<<node_name;
            /*if (farm_id == 0 || farm_id == 1 ){
                DLOG(ERROR)<<"Proxy::GetReadChannel() get slave node_name="<<node_name
                    <<" hash_value"<<hash_value<<" index="<<index;
            }*/
        }
    }

    //2,get channle for read
    std::map<std::string, boost::shared_ptr<ldd::net::Channel> >::iterator pos; 
    ldd::util::RWMutexLock lock(&rw_mutex_);
    pos = nodes_.find(node_name);
    if (pos == nodes_.end()) {
        LOG(ERROR) << "get read channel error";
        return null_channel;
    }else{
        if (proxy_stat_){
            std::map<std::string, int64_t>::iterator itr = read_stat_.find(node_name);
            if (itr != read_stat_.end()){
                (itr->second)++;
                LOG(INFO)<<" add name="<<node_name<<" num="<<itr->second;
            }else{
                read_stat_.insert(std::make_pair(node_name, 1));
                LOG(INFO)<<" add name="<<node_name;
            }
        }
    }

    return pos->second;
}

boost::shared_ptr<Channel> Proxy::GetWriteChannel(int farm_id){
    boost::shared_ptr<Channel> null_channel;

    //1,get node name
    Roles* roles = NULL;
    int ret = configure_->GetRoles(farm_id, &roles);
    if (ret != 0) {
        LOG(ERROR) << "get write roles error";
        return null_channel;
    }

    //2,get channle for read
    std::map<std::string, boost::shared_ptr<ldd::net::Channel> >::iterator pos; 
    ldd::util::RWMutexLock lock(&rw_mutex_);
    pos = nodes_.find(roles->master);
    if (pos == nodes_.end()) {
        LOG(ERROR) << "get write channel error";
        return null_channel;
    }else{
        if (proxy_stat_)
        {
            LOG(INFO)<<"Proxy::GetWriteChannel()";
            std::map<std::string, int64_t>::iterator itr = write_stat_.find(roles->master);
            if (itr != write_stat_.end()){
                (itr->second)++;
                LOG(INFO)<<" add name="<<roles->master<<" num="<<itr->second;
            }else{
                write_stat_.insert(std::make_pair(roles->master, 1));
                LOG(INFO)<<" add name="<<roles->master;
            }
        }
    }

    return pos->second;
}


bool Proxy::Init(){
    if (!InitClient()) {
        return false;
    }

    if (!P2sInit()) {
        return false;
    }

    return true;
}


bool Proxy::InitClient(){
    client_ = new ldd::net::Client(option_);
    return true;
}

void Proxy::HandleError(int error_code){
    LOG(ERROR)<<"Proxy::HandleError() exit(1)";
    exit(1);
}


bool Proxy::P2sInit(){
    //创建configure
    Configure* configure = NULL;
    
    courier_option_.timeout = zookeeper_timeout_;
    courier_option_.cb = boost::bind(&Proxy::HandleError, this, _1);
    int ret = Configure::Init(courier_option_, home_host_port_,
                            zookeeper_host_,    zookeeper_rootpath_,
                            &configure);

    if (ret != 0 || configure == NULL) {
        LOG(ERROR) << "create config info error";
        return false;
    }
    configure_.reset(configure);

    std::vector<std::string> node_list;
    configure_->GetNodeList(boost::bind(&Proxy::NodeChangeCallBack
                , this, _1, _2, _3)
            , &node_list);
    if (node_list.size() <= 0) {
        LOG(ERROR) << "no storage node can be used";
        return false;
    }

    for (std::size_t i = 0; i < node_list.size(); i++) {
        bool ok = ConnectNode(node_list[i]);
        if (!ok) {
            LOG(ERROR) << "connect to node " << node_list[i] << " failed";
            return false;
        }
    }

    return true;
}

bool Proxy::ConnectNode(const std::string &node){
    std::map<std::string, boost::shared_ptr<ldd::net::Channel> >::iterator pos;
    ldd::util::RWMutexLock lock(&rw_mutex_, true);
    pos = nodes_.find(node);
    if (pos != nodes_.end()) {
        LOG(ERROR) << "node " << node << " has connected";
        return false;
    }

    std::vector<std::string> host_port;
    boost::split(host_port, node, boost::is_any_of(":"));
    if (host_port.size() != 2) {
        LOG(ERROR) << "invalid node: " << node;
        return false;
    }

    int port = boost::lexical_cast<int>(host_port[1]);
    if (port <= 0 || port > 65535) {
        LOG(ERROR) << "invalid node port: " << port;
        return false;
    }

    boost::shared_ptr<Channel> channel = client_->Create(
                                    event_loop_, host_port[0], port);

    if (channel.get() == NULL) {
        LOG(ERROR) << "connect to " << node << "failed";
        return false;
    }

    nodes_.insert(std::make_pair(node, channel));

    return true;
}

bool Proxy::RemoveNode(const std::string &node){
    std::map<std::string, boost::shared_ptr<ldd::net::Channel> >::iterator pos;
    ldd::util::RWMutexLock lock(&rw_mutex_, true);
    pos = nodes_.find(node);
    if (pos == nodes_.end()) {
        LOG(ERROR) << "node(" << node << ") is not exist";
        return false;
    }

    nodes_.erase(pos);
    
    return true;
}

std::vector<int>*  Proxy::GetFarmList(){
    std::vector<int> *farm_list;
    int ret = configure_->GetFarmList(&farm_list);
    if (ret != 0) {
        LOG(ERROR) << "get farm list error: " << ret;
        return NULL;
    }

    return farm_list;
}

