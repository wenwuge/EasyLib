// client.cc (2013-08-11)
// Yan Gaofeng (yangaofeng@360.cn)

#include <boost/bind.hpp>
#include <glog/logging.h>
#include <algorithm>
#include <qlog/qlog.h>
#include "gateway_module.h"
#include "user.h"


namespace lcs { namespace gateway {

User::User(GateWayModule *gateway
        , const boost::shared_ptr<ldd::net::Channel> &channel
        , Ship *ship
        , int timeout) 
    : stat_(LOGIN_FIRST_STEP)
      , gateway_(gateway)
      , channel_(channel)
      , ship_(ship)
      , timeout_ms_(timeout)
      , prev_time_(0)
      , request_count_(0)
      , total_request_(0)
      , success_request_(0)
      , byond_user_connect_limit_(false)
      , login_time_(0)
      //, last_user_server_port_(0)

{
    name_ = channel_->name();
    ip_ = channel_->peer_endpoint().address().ToU32();
    port_ = channel_->peer_endpoint().port();

    login_time_ = ship_->time_cache();

    gateway_->IncUsers();

    if (!gateway_->gateway_option().echo
            && gateway_->gateway_option().use_login) {
        StartFirstPacketTimer();
    }
}

User::~User() {

    gateway_->DecUsers();


    //LOG(ERROR) << "debug user destructor: "
    //    << name_
    //    << ", user addr: "
    //    << this;

    //这里必须调用一次，因为有可能是非法用法
    //非法用户不会触发撤消定时器事件
    CancelFirstPacketTimer();

    time_t logout_time = ship_->time_cache();
    time_t used_time = logout_time - login_time_;
    if (used_time >= 0) {
        qLogInfo("CloudSafeLine.GateWay.User"
                //, "%s\t%d\t%s\t %lu\t%lu\t%lu\t %d\t%d"
                , "ip=%s\tport=%d\tmid=%s\t"
                  "login=%lu\tlogout=%lu\tused=%lu\t"
                  "total_req=%d\tsuccess_req=%d\t"
                  "total_count=1"
                , ip().c_str()
                , port()
                , (mid().empty() ? "none" : mid().c_str())
                , login_time_
                , logout_time
                , used_time
                , total_request_
                , success_request_);
    }
    else {
        LOG(ERROR) << "user dtor, invalid user login(" << login_time_
            << ") and logout("
            << logout_time
            << ") time, name: " << name_;
    }
}

ErrorCode User::SendReq(const boost::shared_ptr<ldd::net::OutgoingMsg> &msg
        , std::set<void *> *retried_channels
        , std::string *used_server
        , int *used_server_port
        , int *used_server_stat)
{
    CHECK_NOTNULL(retried_channels);
    CHECK_NOTNULL(used_server);
    CHECK_NOTNULL(used_server_port);
    CHECK_NOTNULL(used_server_stat);

    total_request_++;

    size_t channel_count = 0;
    size_t retry = 0;

REGET:
    //get service channel by msg type
    boost::shared_ptr<ldd::net::Channel> channel 
        = ship_->GetServer(msg->type(), &channel_count);
    if (!channel) {
        *used_server = "none";
        *used_server_port = 0;
        *used_server_stat = 0;
        if (gateway_->IsValidProtocol(msg->type())) {
            //服务器不存在
            LOG(ERROR) << "no server for user: " 
                << name_ 
                << ", mid: "
                << mid()
                << " msg type: " << msg->type();
            return ErrorCode::Server();
        } else {
            //协议错误
            LOG(ERROR) << "user: " 
                << name_ 
                << ", mid: "
                << mid()
                << " msg type: " << msg->type()
                << ", error: no support msg type "
                << msg->type();
            return ErrorCode::MessageType();
        }
    }
    
    *used_server = channel->peer_endpoint().address().ToString();
    *used_server_port = channel->peer_endpoint().port();
    *used_server_stat = channel->state();

    if (!retried_channels->empty()) {
        if (retried_channels->count(channel.get()) != 0) {
            retry++;
            if (retry >= channel_count) {
                LOG(ERROR) << "user" << name_
                    << ", mid: " 
                    << mid()
                    << ", msg type: "
                    << msg->type()
                    << ", retry all("
                    << channel_count
                    << ") channels";
                return ErrorCode::Server();
            }

            DLOG(INFO) << "get a usable server when retry send request, server: "
                << channel->name();
            goto REGET;
        }
    }

    //DLOG(INFO) << "send request to server: " << channel->name()
    //    << ", channel count: " << channel_count;

    //send to service
    //note 
    if (!channel->Post(msg, 
                ldd::util::TimeDiff::Milliseconds(
                    gateway_->server_option().request_send_timeout))) {

        LOG(ERROR) << "send reqeust to server failed, user: "
            << name_
            << ", mid: "
            << mid()
            << ", msg type: "
            << msg->type()
            << ", invalid channel status: "
            << ChannelStatDesc(channel->state());

        return ErrorCode::Server();
    }

    success_request_++;

    //如果超过每秒请求数限制，那么关闭channel
    request_count_++;
    if (request_count_ > gateway_->user_option().request_limit_peer_second) {
        DLOG(WARNING) << "byond user request limit peer second: "
            << request_count_;
        //channel_->Close();
        return ErrorCode::UserRequestLimit();
    }

    if (ship_->time_cache() - prev_time_ >= 1) {
        request_count_ = 0;
        prev_time_ = ship_->time_cache();
    }

    //DLOG(INFO) << "add to retried channels: " << channel.get();
    retried_channels->insert((void *)channel.get());

    return ErrorCode::Ok();
}

ErrorCode User::SendPushMsg(const boost::shared_ptr<ldd::net::OutgoingMsg> &msg)
{
    //LOG(WARNING) << "push messagge send timeout: " << gateway_->push_request_send_timeout();
    if(!channel_) {
        LOG(ERROR) << "emptt user channel send push to user mid  "<< mid_ << "failed";
        return ErrorCode::Server();
    }
    if (!channel_->Post(msg
                , ldd::util::TimeDiff::Milliseconds(
                    gateway_->push_option().send_timeout))) {
        LOG(ERROR) << "send push message to user failed, user: "
            << channel_->name()
            << ", connect status: "
            << ChannelStatDesc(channel_->state());
        return ErrorCode::Server();
    }

    return ErrorCode::Ok();
}

void User::FirstPacketTimeoutHandler()
{
    if (!timer_) {
        LOG(FATAL) << "invalid timer for user timeout";
        return;
    }

    timer_.reset();

    std::string status = channel()->IsConnected() ? "connected" : "closed";

    //触发UserClosed回调,由UserClosed删除用户
    LOG(INFO) << "user first packet timeout and close channel: " 
        << name_
        << ", timeout value: "
        << timeout_ms_
        << ", channel status: "
        << status
        << ", channel addr: "
        << channel().get()
        << ", user addr: "
        << this;

    channel_->Close(); 
}

void User::set_stat(int stat)
{
    if (!gateway_->gateway_option().echo) {
        stat_ = stat;
    }

    return;
}



std::string User::ChannelStatDesc(int stat)
{
    std::string stat_str;
    switch (stat) {
        case ldd::net::Channel::kClosed:
            stat_str = "closed";
            break;
        case ldd::net::Channel::kConnecting:
            stat_str = "connecting";
            break;
        case ldd::net::Channel::kConnected:
            stat_str = "connected";
            break;
        default:
            stat_str = "unknown";
            break;

    }

    return stat_str;
}


void User::StartFirstPacketTimer()
{
    if (timer_) {
        LOG(FATAL) << "repeat start first packet timer";
        return;
    }

    //echo消息，不设置定时器, 因为没有登录流程，不会产生状态
    timer_.reset(new ldd::net::TimerEvent(ship_->event_loop()));
    struct timeval tv;
    tv.tv_sec = timeout_ms_ / 1000;
    tv.tv_usec = (timeout_ms_ % 1000) * 1000;

    timer_->AsyncWait(boost::bind(&User::FirstPacketTimeoutHandler, this)
            , ldd::util::TimeDiff(tv));
}

void User::CancelFirstPacketTimer()
{
    if (timer_) {
        timer_->Cancel();
        timer_.reset();
    }
}

void User::set_mid(const std::string &mid) {
    mid_ = mid;
    //ship_->AddNewUser();
}



//void User::Send

} /*gateway*/
} /*lcs*/

