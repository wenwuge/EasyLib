// session_msg.cc (2014-02-12)
// Yan Gaofeng (yangaofeng@360.cn)
#include <glog/logging.h>
#include "session_msg.h"
#include "proto/lcs_extra.pb.h"
#include <ldd/net/channel.h>
#include <ship.h>

namespace lcs { namespace gateway {

ldd::util::Atomic<uint64_t> ToSession::next_id_;
ldd::util::Atomic<uint64_t> ToSession::free_id_;

bool ToSession::Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout)
{
    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    *done_timeout = *recv_timeout;

    init_time_ = ldd::util::Timestamp::Now();
    request->SetBody(msg_content_);

    return true;
}

bool ToSession::Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout)
{
    //node response message of push server
    //LOG(INFO) << "recv push server response message";

    if (response.extras().empty()) {
        return true;
    }

    std::map<uint8_t, ldd::net::Buffer>::const_iterator pos;
    //extras.empty() 表示操作成功,否则表示服务器返回错误码，需要处理
    //反序列化extra，解析错误码
    pos = response.extras().find(lcs::extra::Result::kID);
    if (pos == response.extras().end()) {
        LOG(ERROR) << channel()->name()
            << "msg type:"
            << type()
            << ", msg id: "
            << id()
            << ", not found result extra with extra id: "
            << lcs::extra::Result::kID;
        return true;
    }

    lcs::extra::Result res;
    if (!res.ParseFromString(pos->second.ToString())) {
        LOG(ERROR) << channel()->name()
            << "msg type: "
            << type()
            << ", msg id: "
            << id()
            << ", unserialize result failed";
        return true;
    }

    switch (res.ec()) {
        case lcs::extra::Result::kOK:
            //LOG(INFO) << channel()->name() 
            //    << ", msg type: "
            //    << type()
            //    << ", to server msg id: "
            //    << id()
            //    << ", success";
            break;
        case lcs::extra::Result::kInvalidRequest:
            LOG(ERROR) << channel()->name() 
                << ", msg type: "
                << type()
                << ", to server msg id: "
                << id()
                << ", server error: invalid request";
            break;
        case lcs::extra::Result::kInvalidClientPublicKey:
            LOG(ERROR) << channel()->name() 
                << ", msg type: "
                << type()
                << ", to server msg id: "
                << id()
                << ", server error: invalid client public key";
            break;
        case lcs::extra::Result::kInvalidClientSymmetricKey:
            LOG(ERROR) << channel()->name() 
                << ", msg type: "
                << type()
                << ", to server msg id: "
                << id()
                << ", server error: invalid client symmetric key";
            break;
        case lcs::extra::Result::kTooManyConnections:
            LOG(ERROR) << channel()->name() 
                << ", msg type: "
                << type()
                << ", to server msg id: "
                << id()
                << ", server error: too many connections";
            break;
        case lcs::extra::Result::kRetryReq:
            LOG(ERROR) << channel()->name() 
                << ", msg type: "
                << type()
                << ", to server msg id: "
                << id()
                << ", server error: retry req for debug";
            break;
        default:
            LOG(ERROR) << channel()->name() 
                << ", msg type: "
                << type()
                << ", to server msg id: "
                << id()
                << ", server error: unknown error";
            break;
    }

    return true;
}

void ToSession::Done(const Result& result)
{
    if (result.IsOk()) {
        return;
    }
    //重新发送不记log
    if(ship_->SendSession(msg_content_.ToString(), channel(), ++retried_times_)) {
    }

    ldd::util::Timestamp done_time = ldd::util::Timestamp::Now();
    ldd::util::TimeDiff ctor2init = init_time_ - ctor_time_;
    ldd::util::TimeDiff init2done = done_time - init_time_;
    ldd::util::TimeDiff ctor2done = done_time - ctor_time_;
  
    std::string channel_name("[unset channel] ");
    boost::shared_ptr<ldd::net::Channel> c = channel();
    if (!result.IsFailed() && c != NULL) {
        channel_name = c->name();
    } 

    LOG(ERROR) << channel_name
        << "send user info to push server error: "
        << result.ToString()
        << ", msgid:" << id()
        << ", ctor2init:"
        << ctor2init.ToMilliseconds()
        << ", init2done: "
        << init2done.ToMilliseconds()
        << ", total used time: "
        << ctor2done.ToMilliseconds()
        << ", retried_times_: "
        << retried_times_;
}

} /*gateway*/
} /*lcs*/

