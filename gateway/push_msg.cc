// push_msg.cc (2013-08-11) // Yan Gaofeng (yangaofeng@360.cn)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <glog/logging.h>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include <qlog/qlog.h>

#include "ldd/net/event_loop.h"
#include "gateway_module.h"
#include "push_msg.h"
#include "proto/lcs_common.pb.h"
#include "user.h"
#include "osl/include/base64.h"


namespace lcs { namespace gateway {

using namespace std;
using namespace ldd::net;


ldd::util::Atomic<uint64_t> PushMsg::next_id_;
ldd::util::Atomic<uint64_t> PushMsg::free_id_;

ldd::util::Atomic<uint64_t> ToClient::next_id_;
ldd::util::Atomic<uint64_t> ToClient::free_id_;

PushMsg::PushMsg(GateWayModule *gateway, Ship *ship) 
            : gateway_(gateway)
              , ship_(ship)
              , error_code_(ErrorCode::Ok())
              , total_count_(0)
              , complete_count_(0)
              , failed_count_(0)
              , timeout_count_(0)
              , local_total_count_(0)
              , local_complete_count_(0)
              , local_failed_count_(0)
              , local_timeout_count_(0)
              , push_user_count_(0)
              , push_type_(0)
              , push_task_id_(0)
              , response_timer_(ship->event_loop())
              , used_time_(0)
{
    next_id_++;
    ctor_time_ = ldd::util::Timestamp::Now();
    init_time_ = ctor_time_;
    emit_time_ = ctor_time_;
    done_time_ = ctor_time_;
}

PushMsg::~PushMsg()
{
    free_id_++;
}


void PushMsg::Init(const Payload& request, Action* next)
{
    //LOG(INFO) << "push task init";

    init_time_ = ldd::util::Timestamp::Now();

    //1,解析push，单播或单播
    request_str_ = request.body().ToString();
    if (!push_.ParseFromString(request_str_)) {
        LOG(ERROR) << channel()->name()
            << "msg type: "
            << type()
            << ", msg id: "
            << id()
            << ", unserialize push message failed"
            << "\tdata :"
            << osl::Base64::encode(request_str_);
        error_code_ = ErrorCode::Request();
        *next = kDone;
        return;
    }

    msg_content_ = ldd::net::Buffer(push_.message());
    push_type_ = push_.type();
    push_user_count_ = push_.cids().size();
    push_task_id_ = push_.taskid();

    LOG(INFO) << channel()->name()
        << "receive a push message, msg type: " << type()
        << ", msg id: " << id()
        << ", push type: " << push_type_
        << ", push user count: " << push_user_count_ 
        << ", push task id: " << push_task_id_;

    switch (push_.type()) {
        case lcs::push::Message::kBroadcast:
            SendBroadcast();
            break;
        case lcs::push::Message::kMulticast:
            //组播消息，暂不实现
            SendMulticast(request.body());
            break;
        case lcs::push::Message::kUnicast:
            //单播消息
            if (!SendUnicast(request.body())) {
                error_code_ = ErrorCode::Request();
                *next = kDone;
                return;
            }
            break;
        default:
            LOG(ERROR) << channel()->name()
                << "msg type: " << type()
                << ", msg id: " << id()
                << ", push type: " << push_type_
                << ", push user count: " << push_user_count_ 
                << ", push task id: " << push_task_id_
                << ", invalid push msg type: " << push_.type();
            error_code_ = ErrorCode::Request();
            *next = kDone;
            return;
            break;
    }

    StartResponseTimer();

    *next = kWait;

    return;
}

void PushMsg::StartResponseTimer()
{
    response_timer_.AsyncWait(
            boost::bind(&PushMsg::ResponseTimerHandler, this),
            ldd::util::TimeDiff::Milliseconds(gateway_->push_option().response_interval));
}

void PushMsg::ResponseTimerHandler()
{
    used_time_ += gateway_->push_option().response_interval;

    Notify();
}
void PushMsg::RecordSucPushMsg(User *user) {
    if(!res_) {
        res_.reset(new lcs::push::Response);
    }
    lcs::push::Report *report = res_->mutable_report();
    lcs::push::UserInfo *  user_info = report->add_user_infos();
    user_info->set_mid(user->mid());
    user_info->set_taskid(push_.taskid());

}

void PushMsg::Emit(Payload* response, Action* next)
{

    emit_time_ = ldd::util::Timestamp::Now();
    *next = kWait;
    if (complete_count_ > 0) {
        //note,应该不需要给服务器返回消息
        res_->set_taskid(push_task_id_);
        res_->set_total(total_count_);
        res_->set_complete(complete_count_);
        res_->set_failed(failed_count_);
        res_->set_timeout(timeout_count_);

        std::string str;
        if (!res_->SerializeToString(&str)) {
            LOG(ERROR) << channel()->name()
                << "msg type: " << type()
                << ", msg id: " << id()
                << ", serizlize push response failed, push type: "
                << push_type_
                << ", push user count: " << push_user_count_
                << ", push task id: " << push_task_id_;
            error_code_ = ErrorCode::MessageType();
            *next = kDone;
            return;
        } else  {
            LOG(INFO) << channel()->name()
                << "msg type: "
                << type()
                << ", msg id: "
                << id()
                << ", responce is successful, task id: " 
                << push_task_id_;

        }
        response->SetBody(ldd::net::Buffer(str));
        *next = kEmit;
    }

    local_total_count_ += total_count_;
    local_complete_count_ += complete_count_;
    local_failed_count_ += failed_count_;
    local_timeout_count_ += timeout_count_;

    if (total_count_ == 0) {

        if (used_time_ > gateway_->push_option().recv_timeout) {
            LOG(INFO) << channel()->name()
                << "msg type: "
                << type()
                << ", msg id: "
                << id()
                << ", push task timeout, set it to end, task id: " 
                << push_task_id_;
            *next = kDone;
            return;
        }

        uint64_t total = local_complete_count_ 
            + local_failed_count_ 
            + local_timeout_count_;

        if (local_total_count_ == total) {
            LOG(INFO) << channel()->name()
                << "msg type: "
                << type()
                << ", msg id: "
                << id()
                << ", push task is successful, task id: " 
                << push_task_id_;
            *next = kDone;
            return;
        }
    }

    total_count_ = 0;
    complete_count_ = 0;
    failed_count_ = 0;
    timeout_count_ = 0;

    res_.reset(new lcs::push::Response);
    StartResponseTimer();

    return;
}

void PushMsg::Done(Code* code)
{
    done_time_ = ldd::util::Timestamp::Now();

    LOG(INFO)
        << "push task done, msg id: "
        << id()
        << ", push task id: "
        << push_task_id_
        << ", total user: "
        << local_total_count_
        << ", push success: "
        << local_complete_count_
        << ", push error: "
        << local_failed_count_
        << ", push timeout: "
        << local_timeout_count_;

    PrintMsgDetail();

    *code = error_code_.code();

    response_timer_.Cancel();
}

void PushMsg::SendBroadcast()
{
    //所有的客户端, 需要gateway提供一个遍历接口

    ship_->Broadcast(boost::static_pointer_cast<PushMsg>(shared_from_this()));
}

void PushMsg::PushMsgToUser(User *user)
{
    CHECK_NOTNULL(user);

    boost::shared_ptr<ToClient> msg 
        = boost::make_shared<ToClient>(
                boost::static_pointer_cast<PushMsg>(shared_from_this())
                , type()
                , msg_content_
                , gateway_->push_option().recv_timeout
                , user);
    if (!msg) {
        LOG(ERROR) << channel()->name()
            << "msg type:"
            << type()
            << ", msg id: "
            << id()
            << ", create msg for client failed";
        return;
    }

    total_count_++;

    ErrorCode e = user->SendPushMsg(msg);
    if (!e.IsOk()) {
        LOG(ERROR) << channel()->name()
            << "msg type: "
            << type()
            << ", msg id: "
            << id()
            << ", send boradcast message to user "
            << user->name()
            << " error: "
            << e.ToString();
        failed_count_++;
    }

}

void PushMsg::SendMulticast(const Buffer &buff)
{
    RepeatedClientID cids = push_.cids();
    RepeatedClientID::iterator it =  cids.begin();
    for (; it != cids.end(); it++) {
        //1,根据CID，找出客户端channel
        struct in_addr in = {};
        //in.s_addr = it->ipv4();
        //uint32_t ip = ntohl(in.s_addr);
        uint32_t ip = it->ipv4();
        int port = it->port();

        boost::shared_ptr<User> user 
            = ship_->GetUser(ip, port);
        if (!user) {
            string addr(inet_ntoa(in));
            LOG(INFO) << channel()->name()
                << " ip: "
                << ip
                << " port "
                << port
                << "msg type: "
                << type()
                << ", msg id: "
                << id()
                << ", push type: " << push_type_
                << ", push user count: " << push_user_count_
                << ", push task id: : " << push_task_id_
                << ", not found user when send multicast msg, may be offlien, user: " 
                << addr << ":" << port;
            continue;
        }

        //2,发送推送消息
        //note, need rewrite 构造outgoingMsg,发送给客户端
        PushMsgToUser(user.get());
    }
}

bool PushMsg::SendUnicast(const Buffer &buff)
{
    //1,解析CID
    if (push_user_count_ != 1) {
        LOG(ERROR) << channel()->name()
            << "msg type: " << type()
            << ", msg id: " << id()
            << ", push type: " << push_type_
            << ", push user count: " << push_user_count_
            << ", push task id: " << push_task_id_
            << ", invalid push user count when send unicast message";
        return false;
    }

    SendMulticast(buff);

    return true;
}

void PushMsg::PrintMsgDetail()
{
    ldd::util::TimeDiff ctor2init = init_time_ - ctor_time_;
    ldd::util::TimeDiff init2emit = emit_time_ - init_time_;
    ldd::util::TimeDiff emit2done = done_time_ - emit_time_;
    ldd::util::TimeDiff init2done = done_time_ - init_time_;

    LOG(INFO) << channel()->name()
        << "msg type: " << type()
        << ", msg id: " << id() 
        << ", push type: " << push_type_
        << ", push user count: " << push_user_count_
        << ", push task id: " << push_task_id_
        << ", ctor 2 init:" << ctor2init.ToMilliseconds()
        << "ms, init 2 emit: " << init2emit.ToMilliseconds()
        << "ms, emit 2 done: " << emit2done.ToMilliseconds()
        << "ms, init 2 done: " << init2done.ToMilliseconds()
        << "ms";
}

bool ToClient::Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout)
{
    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    *done_timeout = *recv_timeout;

    init_time_ = ldd::util::Timestamp::Now();
    //填充客户端请求
    request->SetBody(msg_content_);

    return true;
}

bool ToClient::Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout)
{
    //node 客户端返回消息，这里不处理
    //DLOG(INFO) << "clent msg returned";

    return true;
}

void ToClient::Done(const Result& result)
{
    boost::shared_ptr<PushMsg> parent = parent_.lock();
    if (!parent) {
        LOG(WARNING) << channel_name()
            << " msg type: "
            << type()
            << ", msg id: "
            << id()
            << ", push message to user error: push task has ended";
//        return;
        goto qlog;
    }

    if (result.IsOk()) {
        //推送成功
        parent->complete_count_++;
        parent->RecordSucPushMsg(user_);
    }
    else if (result.IsTimedOut()) {
        //推送超时
        parent->timeout_count_++;
        ldd::util::Timestamp done_time = ldd::util::Timestamp::Now();
        ldd::util::TimeDiff ctor2init = init_time_ - ctor_time_;
        ldd::util::TimeDiff init2done = done_time - init_time_;
        ldd::util::TimeDiff ctor2done = done_time - ctor_time_;
        
        //LOG(ERROR) << "send push message to user "
        //    << channel_name()
        //    << " error: "
        //    << " send time out, ctor2init:"
        //    << ctor2init.ToMilliseconds()
        //    << ", init2done: "
        //    << init2done.ToMilliseconds()
        //    << ", total used time: "
        //    << ctor2done.ToMilliseconds();
    }
    else {
        //推送失败
        parent->failed_count_++;
        //LOG(ERROR) << "send push message to user "
        //    << channel_name()
        //    << " error: "
        //    << result.ToString();
    }

    if (parent->total_count_ >= 
            (uint64_t)parent->gateway_->push_option().response_threshold) {
        parent->Notify();
    }

    //不能结束PushMsg，因为可能是广播消息，或多播消息
    //PushMsg的生存期应该在所有的客户端消息推送完成后
    //parent_->Notify(true);
    //
qlog:
    if (parent && !parent->gateway_->push_option().qlog_info) {
        return;
    }
    std::string client_ip;
    if(channel()) {
        client_ip = channel()->peer_endpoint().address().ToString().c_str();
    }
    qLogInfo("CloudSafeLine.GateWay.Push"
            //, "%s\t%s\t %d\t%s\t %lld\t %d\t%s"
            , "push_srv_ip=%s\tuser_cli_ip=%s\t"
              "msg_type=%d\tuser_mid=%s\t"
              "send_time=%lld\t"
              "res_code=%d\tres_str=%s\t"
              "total_count=1"
            , client_ip.c_str()
            , user_->ip().c_str()
            , type()
            , (user_->mid().empty() ? "none" : user_->mid().c_str())
            , (long long int)init_time_.ToMilliseconds()
            , result.code()
            , result.ToString().c_str());
}

std::string ToClient::channel_name()
{
    boost::shared_ptr<ldd::net::Channel> c = channel();
    if (c) {
        return c->name();
    }

    return std::string("[unset channel]");
}

} /*gateway*/
} /*lcs*/

