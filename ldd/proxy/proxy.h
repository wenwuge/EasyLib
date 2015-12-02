// proxy.h (2013-07-11)
// Yan Gaofeng (yangaofeng@360.cn)

#ifndef __PROXY_H__
#define __PROXY_H__

#include <map>
#include <boost/shared_ptr.hpp>

#include "ldd/util/mutex.h"
#include "ldd/net/client.h"

#include "ldd/util/slice.h"
#include "ldd/courier/configure.h"

#include "ldd/net/client.h"
#include "ldd/net/event_loop.h"

class Proxy {
public:
    Proxy(ldd::net::EventLoop* event_loop,
         ldd::net::Client::Options& client_opt,const std::string& home_host_port,
          const std::string& zookeeper_host,
          const std::string& zookeeper_rootpath,
          int zookeeper_timeout,
          int recv_timeout, 
          int done_timeout,
          int role_type,
          bool proxy_stat);
    ~Proxy();

    bool Init();

    int GetNameSpaceID(const std::string &ns);
    std::size_t GetHashValue(const ldd::util::Slice &key);
    int GetBucketID(std::size_t hash_value);
    int GetFarmID(int bucket_id);
    std::vector<int> *GetFarmList();
    boost::shared_ptr<ldd::net::Channel> GetReadChannel(std::size_t hash_value
            , int farm_id);
    boost::shared_ptr<ldd::net::Channel> GetWriteChannel(int farm_id);

    void HandleError(int error_code);

    int get_recv_timeout(){
        return recv_timeout_;
    }

    int get_done_timeout(){
        return done_timeout_;
    }

private:
    bool InitClient();
    bool P2sInit();

    bool ConnectNode(const std::string &node);
    bool RemoveNode(const std::string &node);
    void NodeChangeCallBack(int ret
            , ldd::courier::NodeChangedType type
            , const std::string& node);

private:

    ldd::net::Client* client_;
    ldd::net::EventLoop* event_loop_;
    ldd::net::Client::Options option_;

    boost::shared_ptr<ldd::courier::Configure> configure_; 

    //节点名称: type: std::string, value: 'host:port'
    ldd::util::RWMutex rw_mutex_;
    std::map<std::string, boost::shared_ptr<ldd::net::Channel> > nodes_;

    std::map<std::string, int64_t> read_stat_;
    std::map<std::string, int64_t> write_stat_ ;

    // add on 9.24
    int pulse_interval_;                     

    std::string home_host_port_;
    std::string zookeeper_host_;
    std::string zookeeper_rootpath_;   
    int zookeeper_timeout_;    
    ldd::courier::Options courier_option_;

    int recv_timeout_;
    int done_timeout_;
    int role_type_;
    bool proxy_stat_;
};

#endif /*__PROXY_H__*/

