// client.h (2013-08-11)
// Yan Gaofeng (yangaofeng@360.cn)
#ifndef __USER_H__
#define __USER_H__

#include <set>
#include <boost/intrusive/list.hpp>

#include <ldd/net/endpoint.h>
#include <ldd/net/event.h>
#include <ldd/net/channel.h>
#include <ldd/net/outgoing_msg.h>

#include "error_code.h"

/*
 * 代表一个客户端连接，其中，first是状态标志，
 * 用来判断第一个包是不是登录包，如果第一个数据包不是登录，直接关闭连接
 * */

namespace lcs { namespace gateway {

#define LOGIN_FIRST_STEP        0
#define LOGIN_SECOND_STEP       1
#define LOGIN_THIRD_STEP        2
#define LOGIN_OTHER_STEP        3

class GateWayModule;
class Ship;



class User : public boost::intrusive::list_base_hook<
            boost::intrusive::link_mode<boost::intrusive::auto_unlink> > {
public:
    User(GateWayModule *gateway
            , const boost::shared_ptr<ldd::net::Channel> &channel
            , Ship *ship
            , int timeout);
    ~User();


    int stat() const { return stat_; }
    void set_stat(int stat);
    const std::string &name() const { return name_; }

    static std::string GetErrorDesc(int ec);
    static std::string ChannelStatDesc(int stat);

    const std::string key() const { return serialized_key_; }

    void set_key(std::string serialized_key) { 
        serialized_key_ = serialized_key;
    }

    ErrorCode SendReq(const boost::shared_ptr<ldd::net::OutgoingMsg> &msg
            , std::set<void *> *retried_channels
            , std::string *used_server
            , int *used_server_port
            , int *used_server_stat);
    ErrorCode SendPushMsg(const boost::shared_ptr<ldd::net::OutgoingMsg> &msg);

    std::string ip() const { return channel_->peer_endpoint().address().ToString(); }
    uint32_t ip_u32() const { return channel_->peer_endpoint().address().ToU32(); }
    uint16_t port() const { return channel_->peer_endpoint().port(); }
    boost::shared_ptr<ldd::net::Channel> channel() { return channel_; }

    void set_byond_connect_limit(bool flag) { 
        byond_user_connect_limit_ = flag; 
    }

    bool byond_connect_limit() const { 
        return byond_user_connect_limit_;
    }

    void set_mid(const std::string &mid);
    const std::string mid() const { return mid_; }

    Ship *ship() { return ship_; }

    void StartFirstPacketTimer();
    void CancelFirstPacketTimer();

private:
    void FirstPacketTimeoutHandler();

private:
    int stat_;
    std::string serialized_key_;
    GateWayModule *gateway_;
    boost::shared_ptr<ldd::net::Channel> channel_;
    Ship *ship_;
    int timeout_ms_;
    boost::scoped_ptr<ldd::net::TimerEvent> timer_;
    uint32_t ip_;
    uint16_t port_;

    time_t prev_time_;
    int request_count_;
    int total_request_;
    int success_request_;

    bool byond_user_connect_limit_;
    std::string name_;
    std::string mid_;

    time_t login_time_; //seconds
};

} /*gateway*/
} /*lcs*/

#endif /*__USER_H__*/

