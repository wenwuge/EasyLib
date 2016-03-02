// default_msg.cc (2013-07-30)
// Yan Gaofeng (yangaofeng@360.cn)
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <glog/logging.h>
#include "proto/lcs_session.pb.h"
#include "proto/lcs_extra.pb.h"

#include <boost/make_shared.hpp>

#include <qlog/qlog.h>

#include <ldd/net/backtrace.h>
#include "ldd/net/event_loop.h"
#include "gateway_module.h"
#include "user.h"
#include "default_msg.h"

namespace lcs {
namespace gateway {

int log_great_than = 100;
uint32_t log_stat_interval = 1;
__thread osl::StatTimeManager *stat_tsm = NULL;
__thread uint32_t success_count = 0;
__thread uint32_t failed_count = 0;
__thread uint32_t fin_010_count = 0;
__thread uint32_t fin_020_count = 0;
__thread uint32_t fin_030_count = 0;
__thread uint32_t fin_040_count = 0;
__thread uint32_t fin_050_count = 0;
__thread uint32_t fin_100_count = 0;
__thread uint32_t fin_200_count = 0;
__thread uint32_t fin_300_count = 0;
__thread uint32_t fin_400_count = 0;
__thread uint32_t fin_500_count = 0;
__thread uint32_t fin_600_count = 0;
__thread uint32_t fin_700_count = 0;
__thread uint32_t fin_800_count = 0;
__thread uint32_t fin_900_count = 0;
__thread uint32_t fin_rst_count = 0;
__thread uint32_t ekRequest     = 0;
__thread uint32_t ekParseIni    = 0;
__thread uint32_t ekFieldV      = 0;
__thread uint32_t ekTooManyUser = 0;
__thread uint32_t ekServer      = 0;
__thread uint32_t ekServerTimeout   = 0;
__thread uint32_t ekFirstLogin  = 0;
__thread uint32_t ekSecondSconf = 0;
__thread uint32_t ekUserConnectLimit = 0;
__thread uint32_t ekUserRequestLimit = 0;
__thread uint32_t ekMessageType = 0;

using namespace std;
using namespace ldd::net;

ldd::util::Atomic<uint64_t> DefaultMsg::next_id_;
ldd::util::Atomic<uint64_t> DefaultMsg::free_id_;

ldd::util::Atomic<uint64_t> ToServer::next_id_;
ldd::util::Atomic<uint64_t> ToServer::free_id_;

DefaultMsg::DefaultMsg(GateWayModule *gateway) 
    : gateway_(gateway)
      , first_(false)
      , error_code_(ErrorCode::Ok())
      , retry_times_(0)
      , first_send_(true)
      , used_server_port_(0)
      , used_server_stat_(0)
{
    next_id_++;

    //ctor_time_ = ldd::util::Timestamp::Now();
    //init_time_ = ctor_time_;
    //emit_time_ = ctor_time_;
    //done_time_ = ctor_time_;

}

DefaultMsg::~DefaultMsg()
{
    free_id_++;
}

void DefaultMsg::Init(const Payload& request, Action* next)
{
    //LOG(INFO) << "default msg, req_buf id: " << req_buf_.id()
    //    << ", res_buf id: " << res_buf_.id();

    init_time_ = ldd::util::Timestamp::Now();

    gateway_->IncRequest();

    if (gateway_->gateway_option().echo && type() == 7) {
        //回显请求
        //DLOG(INFO) << channel()->name()
        //    << ", msg type: "
        //    << type()
        //    << ", msg id: "
        //    << id()
        //    << ", echo msg";
        res_buf_ = request.body();
        *next = kEmit;
        return;
    }

    req_buf_ = request.body();

    if (channel()->context().empty()) {
        LOG(ERROR) << channel()->name()
            << "msg type: "
            << type()
            << ", msg id: "
            << id()
            << ", not found user context";
        *next = kDone;
        return;
    }

    user_ = boost::any_cast<boost::weak_ptr<User> >(channel()->context());
    boost::shared_ptr<User> user = user_.lock();

    if (!user) {
        LOG(ERROR) << channel()->name()
            << "msg type: "
            << type()
            << ", msg id: "
            << id()
            << ", can not get user from channel context";

        error_code_ = ErrorCode::Server();
        *next = kDone;
        return;
    }

    //在ship中判断, 在default中返回错误消息，关闭连接
    if (user->byond_connect_limit()) {
        error_code_ = ErrorCode::UserConnectLimit();
        *next = kDone;
        return;
    }

    int stat = user->stat();

    if (gateway_->gateway_option().use_login) {
        //第一个数据包必须是login包
        if (stat == LOGIN_FIRST_STEP) {

            if (type() != CLI_LOGIN_MSG) {
                DLOG(ERROR) << channel()->name()
                    << "msg type: " << type()
                    << ", msg id: " << id()
                    << ", first request type must be login request";

                //从客户端列表中删除
                error_code_ = ErrorCode::FirstLogin();
                *next = kDone;
                return;
            }

            user->set_stat(LOGIN_SECOND_STEP);
            stat = user->stat();
            first_ = true;

            user->CancelFirstPacketTimer();
        }
        else if (stat == LOGIN_SECOND_STEP) {
           //第二个数据包必须是sconf
            if (type() != CLI_SCONF_MSG) {
                DLOG(ERROR) << channel()->name()
                    << "msg type: " << type()
                    << ", msg id: " << id()
                    << ", second request type must be sconf";
                error_code_ = ErrorCode::SecondSconf();
                *next = kDone;
                return;
            }

            user->set_stat(LOGIN_THIRD_STEP);
            stat = user->stat();

            //SendSessionNotify();
        }
        else if (stat == LOGIN_THIRD_STEP) {
            //send session to session server
            //note
            user->set_stat(LOGIN_OTHER_STEP);
            stat = user->stat();
        }
        else {
            //LOG(ERROR) << "other packet, type=" << type() 
            //    << ", id=" << id() << ", stat=" << user_->stat();
        }
    }


    if (!SendRequest()) {
        *next = kDone;
    }
    else {
        *next = kWait;
    }
}

bool DefaultMsg::SendRequest()
{
    boost::shared_ptr<User> user = user_.lock();
    if (!user) {
        LOG(ERROR) << channel()->name()
            << "msg type: "
            << type()
            << ", msg id: "
            << id()
            << ", get user failed from channel context when send request";

        error_code_ = ErrorCode::Server();
        //channel()->Close();
        return false;
    }

    boost::shared_ptr<ldd::net::OutgoingMsg> req = boost::make_shared<ToServer>(
                boost::static_pointer_cast<DefaultMsg>(shared_from_this()));

    if (retry_times_ > 0) {
        DLOG(WARNING) << channel()->name()
            << "msg type: "
            << type()
            << ", msg id: "
            << id()
            << ", retry times: "
            << retry_times_;

        if (retry_times_ > gateway_->user_option().retry_times) {
            DLOG(WARNING) << channel()->name()
                << "msg type: " << type()
                << ", msg id: " << id()
                << ", beyond retry times limit: "
                <<  gateway_->user_option().retry_times;
            error_code_ = ErrorCode::Server();
            return false;
        }
    }

    //发送请求到服务器
    error_code_ = user->SendReq(req
            , &retried_channels_
            , &used_server_
            , &used_server_port_
            , &used_server_stat_);

    if (!error_code_.IsOk()) {
        DLOG(ERROR) << channel()->name()
            << "msg type: " << type()
            << ", msg id: " << id()
            << ", send request to "
            << used_server_ << ":" << used_server_port_
            << " error: " 
            << error_code_.ToString();
        return false;
    }

    if (!first_send_) {
        retry_times_++;
    }
    else {
        first_send_ = false;
    }

    return true;
}

void DefaultMsg::Emit(Payload* response, Action* next)
{
    //emit_time_ = ldd::util::Timestamp::Now();

    response->SetBody(res_buf_);

    if (gateway_->gateway_option().echo && type() == 7) {
        *next = kDone;
        return;
    }

    //*next = kDone;
    *next = kWait;
}

inline void stat_fin(osl::StatTime *st, int64_t ms)
{
    if (ms <= 10) {
        fin_010_count++;
    } else if (ms <= 20) {
        fin_020_count++;
    } else if (ms <= 30) {
        fin_030_count++;
    } else if (ms <= 40) {
        fin_040_count++;
    } else if (ms <= 50) {
        fin_050_count++;
    } else if (ms <= 100) {
        fin_100_count++;
    } else if (ms <= 200) {
        fin_200_count++;
    } else if (ms <= 300) {
        fin_300_count++;
    } else if (ms <= 400) {
        fin_400_count++;
    } else if (ms <= 500) {
        fin_500_count++;
    } else if (ms <= 600) {
        fin_600_count++;
    } else if (ms <= 700) {
        fin_700_count++;
    } else if (ms <= 800) {
        fin_800_count++;
    } else if (ms <= 900) {
        fin_900_count++;
    } else {
        fin_rst_count++;
    }
}

void DefaultMsg::Done(Code* code)
{
    bool msg_err = false;
    ldd::util::TimeDiff init2done = ldd::util::Timestamp::Now() - init_time_;

    *code = error_code_.code();

    boost::shared_ptr<User> user = user_.lock();
    if (user) {
        //统计日志
        qLogInfo("CloudSafeLine.GateWay.Request"
                //, "%s\t%d\t %s\t%d\t %s\t%d\t%d\t %lld\t%d\t %d\t%s\t %lld"
                , "user_ip=%s\tuser_port=%d\t"
                  "user_mid=%s\tmsg_type=%d\t"
                  "msg_id=%d\t"
                  "serv_ip=%s\tserv_port=%d\tserv_stat=%d\t"
                  "req_time=%lld\tused_time=%lld\tretry=%d\t"
                  "err_code=%d\terr_str=%s\t"
                  "total_count=1"
                , user->ip().c_str()
                , user->port()
                , (user->mid().empty() ? "none" : user->mid().c_str())
                , type()
                , id()
                , (used_server_.empty() ? "none" : used_server_.c_str())
                , used_server_port_
                , used_server_stat_
                , (long long int)init_time_.ToMilliseconds()
                , (long long int)init2done.ToMilliseconds()
                , retry_times_
                , error_code_.code()
                , error_code_.ToString().c_str());
    } else {
        LOG(ERROR) << channel()->name()
            << "msg type: "
            << type()
            << ", msg id: "
            << id()
            << ", can get user for qlog";
    }

    if (!error_code_.IsOk()) {
        msg_err = true;
        if (error_code_.IsTooManyUser()
            || error_code_.IsFirstLogin()
            || error_code_.IsSecondSconf()
            || error_code_.IsUserConnectLimit()
            || error_code_.IsUserRequestLimit()
            || error_code_.IsMessageType())
        {
            DLOG(ERROR) << channel()->name()
                << "msg type: " << type()
                << ", msg id: " << id() 
                << ", used time: "
                << (long long int)init2done.ToMilliseconds()
                << "ms, close channel, error: "
                << error_code_.ToString();
            //thread safe
            channel()->Close();
        } else {
            LOG(ERROR) << channel()->name()
                << "msg type: " << type()
                << ", msg id: " << id() 
                << ", used time: "
                << (long long int)init2done.ToMilliseconds()
                << "ms, error: "
                << error_code_.ToString();
        }

        if (error_code_.IsReqeust()) {
            ekRequest++;
        } else if (error_code_.IsParseIni()) {
            ekParseIni++; 
        } else if (error_code_.IsFieldV()) {
            ekFieldV++;
        } else if (error_code_.IsTooManyUser()) {
            ekTooManyUser++;
        } else if (error_code_.IsServer()) {
            ekServer++;
        } else if (error_code_.IsServerTimerout()) {
            ekServerTimeout++;
        } else if (error_code_.IsFirstLogin()) {
            ekFirstLogin++;
        } else if (error_code_.IsSecondSconf()) {
            ekSecondSconf++;
        } else if (error_code_.IsUserConnectLimit()) {
            ekUserConnectLimit++;
        } else if (error_code_.IsUserRequestLimit()) {
            ekUserRequestLimit++;
        } else if (error_code_.IsMessageType()) {
            ekMessageType++;
        }
    }

    if (stat_tsm == NULL) {
        stat_tsm = new osl::StatTimeManager(log_stat_interval);
    }

    osl::StatTime* st = stat_tsm->get();
    if (!msg_err) {
        success_count++;

        st->add("tuse.t", init2done.ToMicroseconds()); 
    } else {
        failed_count++;

        st->add("fail.t", init2done.ToMicroseconds()); 
    }

    stat_fin(st, init2done.ToMilliseconds());

    if (stat_tsm->isTimeToDump(st)) {
        osl::string log = st->dump(osl::StatTime::kMilliSecond, 2, "\t");

        qLogInfo("CloudSafeLine.GateWay.Stat"
                , "%s\t"
                  "total_succ=%d\t"
                  "total_fail=%d\t"
                  "retry=%d\t"
                  "000-010=%d\t"
                  "010-020=%d\t"
                  "020-030=%d\t"
                  "030-040=%d\t"
                  "040-050=%d\t"
                  "050-100=%d\t"
                  "100-200=%d\t"
                  "200-300=%d\t"
                  "300-400=%d\t"
                  "400-500=%d\t"
                  "500-600=%d\t"
                  "600-700=%d\t"
                  "700-800=%d\t"
                  "800-900=%d\t"
                  ">900=%d\t"
                  "ekRequest=%d\t"
                  "ekParseIni=%d\t"
                  "ekFieldV=%d\t"
                  "ekTooManyUser=%d\t"
                  "ekServer=%d\t"
                  "ekServerTimeout=%d\t"
                  "ekFirstLogin=%d\t"
                  "ekSecondSconf=%d\t"
                  "ekUserConnectLimit=%d\t"
                  "ekUserRequestLimit=%d\t"
                  "ekMessageType=%d"
                , log.c_str()
                , success_count
                , failed_count
                , retry_times_
                , fin_010_count
                , fin_020_count
                , fin_030_count
                , fin_040_count
                , fin_050_count
                , fin_100_count
                , fin_200_count
                , fin_300_count
                , fin_400_count
                , fin_500_count
                , fin_600_count
                , fin_700_count
                , fin_800_count
                , fin_900_count
                , fin_rst_count
                , ekRequest
                , ekParseIni
                , ekFieldV
                , ekTooManyUser
                , ekServer
                , ekServerTimeout
                , ekFirstLogin
                , ekSecondSconf
                , ekUserConnectLimit
                , ekUserRequestLimit
                , ekMessageType);

        success_count = 0;
        failed_count = 0;
        fin_010_count = 0;
        fin_020_count = 0;
        fin_030_count = 0;
        fin_040_count = 0;
        fin_050_count = 0;
        fin_100_count = 0;
        fin_200_count = 0;
        fin_300_count = 0;
        fin_400_count = 0;
        fin_500_count = 0;
        fin_600_count = 0;
        fin_700_count = 0;
        fin_800_count = 0;
        fin_900_count = 0;
        fin_rst_count = 0;
        ekRequest     = 0;
        ekParseIni    = 0;
        ekFieldV      = 0;
        ekTooManyUser = 0;
        ekServer      = 0;
        ekServerTimeout   = 0;
        ekFirstLogin  = 0;
        ekSecondSconf = 0;
        ekUserConnectLimit = 0;
        ekUserRequestLimit = 0;
        ekMessageType = 0;
    }
}

void DefaultMsg::Cancel() {
    DLOG(WARNING) << channel()->name()
        << "msg type: "
        << type()
        << ", msg id: "
        << id() 
        << ", msg is canceled";
}


ToServer::ToServer(const boost::shared_ptr<DefaultMsg>& default_msg) 
    : parent_(default_msg)
{ 
    next_id_++;
    set_type(default_msg->type());
    parent_id_ = default_msg->id();

    //ctor_time_ = ldd::util::Timestamp::Now();
}

ToServer::~ToServer()
{
    free_id_++;
}

bool ToServer::Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout)
{
    init_time_ = ldd::util::Timestamp::Now();

    boost::shared_ptr<DefaultMsg> parent = parent_.lock();
    if (!parent) {
        DLOG(ERROR) << channel()->name() 
            << "msg type: "
            << type()
            << ", client msg id: " 
            << parent_id_ 
            << ", to server msg id: "
            << id()
            << ", client msg has been released";
        return false;
    }

    *recv_timeout = ldd::util::TimeDiff::Milliseconds(
            parent->gateway_->server_option().request_recv_timeout);
    *done_timeout = *recv_timeout;

    //1，构造客户端上下文
    lcs::extra::ClientContext cc;

    lcs::Channel *channel = cc.mutable_channel();
    //note need to write
    channel->set_gwid(parent->gateway_->id());

    lcs::Channel::ClientID *user_id = channel->mutable_cid();


    string addr = parent->channel()->peer_endpoint().address().ToString();
    //uint32_t ip = parent->channel()->peer_endpoint().address().ToU32();
    struct in_addr in = parent->channel()->peer_endpoint().address().ToV4();
    uint16_t port = parent->channel()->peer_endpoint().port();

    user_id->set_ipv4(in.s_addr);
    //note, 字节序需要考虑
    user_id->set_port(port);

    string serialize;
    if (!cc.SerializeToString(&serialize)) {
        LOG(ERROR) << ToServer::channel()->name()
            << "serialize client context failed, "
            << addr << ":" << port;
        parent->error_code_ = ErrorCode::Server();
        //parent->channel()->Close();
        parent->Notify(true);
        return false;
    }

    //2,填充客户端请求
    request->SetBody(parent->req_buf_);

    //3,填充客户端上下文
    //DLOG(INFO) << "fill user context to request";
    Buffer context(serialize);
    //LOG(INFO) << "default msg, context buf id: " << context.id();
    request->SetExtra(lcs::extra::ClientContext::kID, context);

    if (parent->gateway_->gateway_option().use_login) {
        if (type() == CLI_SCONF_MSG) {
            //添加key到extra
            boost::shared_ptr<User> user = parent->user_.lock();
            if (!user) {
                LOG(ERROR) << ToServer::channel()->name()
                    << "invalid user, may be disconnect";
                return false;
            }

            Buffer key(user->key());
            //LOG(INFO) << "default msg, key buf id: " << key.id();
            request->SetExtra(lcs::extra::RamdonXorKey::kID, key);
        }
    }

    return true;
}

bool ToServer::Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout)
{
    //recv_time_ = ldd::util::Timestamp::Now();

    boost::shared_ptr<DefaultMsg> parent = parent_.lock();
    if (!parent) {
        DLOG(WARNING) << channel()->name()
            << "msg type:"
            << type()
            << ", client msg id: "
            << parent_id_
            << ", to server msg id: "
            << id()
            << ", client msg has released";

        return false;
    }

    std::map<uint8_t, Buffer>::const_iterator pos;

    if (!response.extras().empty()) {
        //extras.size <= 0 表示操作成功,否则表示服务器返回错误码，需要处理
        //反序列化extra，解析错误码
        pos = response.extras().find(lcs::extra::Result::kID);
        if (pos == response.extras().end()) {
            LOG(ERROR) << channel()->name()
                << "msg type:"
                << type()
                << ", client msg id: "
                << parent_id_
                << ", to server msg id: "
                << id()
                << ", not found result extra with extra id: "
                << lcs::extra::Result::kID;
            parent->error_code_ = ErrorCode::Server();
            parent->Notify(true);
            return true;
        }

        lcs::extra::Result res;
        if (!res.ParseFromString(pos->second.ToString())) {
            LOG(ERROR) << channel()->name()
                << "msg type: "
                << type()
                << ", client msg id: "
                << parent_id_
                << ", to server msg id: "
                << id()
                << ", unserialize result failed";
            parent->error_code_ = ErrorCode::Server();
            parent->Notify(true);
            return true;
        }

        if (res.ec() != lcs::extra::Result::kOK) {
            //DLOG(INFO) << channel()->name()
            //    << "msg type: "
            //    << type()
            //    << ", client msg id: "
            //    << parent_id_
            //    << ", to server msg id: "
            //    << id()
            //    << ", process error in recv method";
            ProcessServerError(res.ec());

            if (res.ec() == lcs::extra::Result::kRetryReq) {
                if (parent->SendRequest()) {
                    //LOG(INFO) << channel()->name()
                    //    << "msg type: "
                    //    << type()
                    //    << ", client msg id: "
                    //    << parent_id_
                    //    << ", to server msg id: "
                    //    << id()
                    //    << ", retry send reqeust success";
                    return true;
                } else {
                    DLOG(ERROR) << channel()->name()
                        << "msg type: "
                        << type()
                        << ", client msg id: "
                        << parent_id_
                        << ", to server msg id: "
                        << id()
                        << ", retry send reqeust failed";
                    return false;
                }
            }

            return false;
        }

        //for session
        if (parent->gateway_->gateway_option().use_login
                && type() == CLI_LOGIN_MSG) {

            boost::shared_ptr<User> user = parent->user_.lock();
            if (!user) {
                LOG(ERROR) << channel()->name()
                    << "msg type: "
                    << type()
                    << ", client msg id: "
                    << parent_id_
                    << ", to server msg id: "
                    << id()
                    << ", parent user member is invalid";
                return false;
            }

            user->set_mid(res.mid());
            user->ship()->AddNewUser(user);
        }
    }

    if (parent->gateway_->gateway_option().use_login
            && type() == CLI_LOGIN_MSG) {

        if (response.extras().empty()) {
            LOG(ERROR) << channel()->name()
                << "msg type: "
                << type()
                << "client msg id: "
                << parent_id_
                << "to server msg id: "
                << id()
                << ", server return result should not be empty";
            return false;
        }

        pos = response.extras().find(lcs::extra::RamdonXorKey::kID);
        if (pos == response.extras().end()) {
            LOG(ERROR) << channel()->name()
                << "msg type: "
                << type()
                << "client msg id: "
                << parent_id_
                << "to server msg id: "
                << id()
                << ", not found RamdonXorKey with extra id: " 
                << lcs::extra::RamdonXorKey::kID;
            parent->error_code_ = ErrorCode::FirstLogin();
            parent->Notify(true);
            return true;
        }

        //验证是否可以解析成功
        lcs::extra::RamdonXorKey key;
        if (!key.ParseFromString(pos->second.ToString())) {
            LOG(ERROR) << channel()->name()
                << "msg type: "
                << type()
                << ", client msg id: "
                << parent_id_
                << ", to server msg id: "
                << id()
                << ", unserialize RamdonXorKey failed";
            parent->error_code_ = ErrorCode::FirstLogin();
            parent->Notify(true);
            return true;
        }

        //在client中保存由login服务服务器返回的random xor key
        boost::shared_ptr<User> user = parent->user_.lock();
        if (!user) {
            LOG(ERROR) << channel()->name()
                << "msg type: "
                << type()
                << ", client msg id: "
                << parent_id_
                << ", to server msg id: "
                << id()
                << ", parent user member is invalid";
            return false;
        }

        user->set_key(pos->second.ToString());
    }

    parent->res_buf_ = response.body();

    //2, note, 将结果转发给客户端
    parent->Notify();

    return true;
}

void ToServer::Done(const Result& result)
{
    ldd::util::TimeDiff init2done = ldd::util::Timestamp::Now() - init_time_;

    boost::shared_ptr<DefaultMsg> parent = parent_.lock();
    if (!parent) {
        LOG(ERROR) << channel()->name()
            << "msg type: "
            << type()
            << ", client msg id: "
            << parent_id_
            << ", to server msg id: "
            << id()
            << ", this: " 
            << this
            << ", used time: "
            << (long long int)init2done.ToMilliseconds()
            << ", client msg has been released";

        return;
    }

    do {
        if (result.IsOk()) {
            if (result.code() != 0) {
                DLOG(ERROR) << channel()->name() 
                    << ", msg type: "
                    << type()
                    << ", client msg id: " 
                    << parent_id_ 
                    << ", to server msg id: "
                    << id()
                    << ", used time: "
                    << (long long int)init2done.ToMilliseconds()
                    << ", broker error code: no server to be used";
                parent->error_code_ = ErrorCode::Server();
            }
            break;
        }

        if (result.IsTimedOut()) {
            LOG(WARNING) << channel()->name() 
                << ", msg type: "
                << type()
                << ", client msg id: " 
                << parent_id_ 
                << ", to server msg id: "
                << id()
                << ", used time: "
                << (long long int)init2done.ToMilliseconds()
                << ", server time out, retry another server";
            if (!parent->SendRequest()) {
                DLOG(ERROR) << channel()->name() 
                    << ", msg type: "
                    << type()
                    << ", client msg id: " 
                    << parent_id_ 
                    << ", to server msg id: "
                    << id()
                    << ", retry to send request failed";
                parent->error_code_ = ErrorCode::ServerTimeout();
            }
            return;
        } else if (result.IsFailed()) {
            LOG(WARNING) << channel()->name() 
                << ", msg type: "
                << type()
                << ", client msg id: " 
                << parent_id_ 
                << ", to server msg id: "
                << id()
                << ", used time: "
                << (long long int)init2done.ToMilliseconds()
                << ", server failed, retry another server";
            if (!parent->SendRequest()) {
                DLOG(ERROR) << channel()->name() 
                    << ", msg type: "
                    << type()
                    << ", msg id: " 
                    << parent_id_ 
                    << ", to server msg id: "
                    << id()
                    << ", retry to send request failed";
                parent->error_code_ = ErrorCode::Server();
            }

            return;
        } else if (result.IsCanceled()) {
            DLOG(ERROR) << channel()->name() 
                << ", msg type: "
                << type()
                << ", client msg id: " 
                << parent_id_ 
                << ", to server msg id: "
                << id()
                << ", used time: "
                << (long long int)init2done.ToMilliseconds()
                << ", server canceled";
            parent->error_code_ = ErrorCode::Server();
        }

    } while (0);

    //DLOG(INFO) << channel()->name() 
    //    << ", msg type: "
    //    << type()
    //    << ", client msg id: " 
    //    << parent_id_ 
    //    << ", to server msg id: "
    //    << id()
    //    << ", complete, status code: "
    //    << parent->error_code_.ToString();

    parent->Notify(true);
}

void ToServer::ProcessServerError(int ec)
{
    boost::shared_ptr<DefaultMsg> parent = parent_.lock();
    if (!parent) {
        LOG(ERROR) << channel_name()
            << ", msg type: "
            << type()
            << ", client msg id: " 
            << parent_id_ 
            << ", to server msg id: "
            << id()
            << ", client msg has been released";
        return;
    }

    switch (ec) {
        case lcs::extra::Result::kOK:
            break;
        case lcs::extra::Result::kInvalidRequest:
            parent->error_code_ = ErrorCode::Request();
            DLOG(ERROR) << channel_name()
                << ", msg type: "
                << type()
                << ", client msg id: " 
                << parent_id_ 
                << ", to server msg id: "
                << id()
                << ", server error: invalid request";
            //parent->Notify(true);
            break;
        case lcs::extra::Result::kInvalidClientPublicKey:
            parent->error_code_ = ErrorCode::Request();
            DLOG(ERROR) << channel_name()
                << ", msg type: "
                << type()
                << ", client msg id: " 
                << parent_id_ 
                << ", to server msg id: "
                << id()
                << ", server error: invalid client public key";
            //parent->Notify(true);
            break;
        case lcs::extra::Result::kInvalidClientSymmetricKey:
            parent->error_code_ = ErrorCode::Request();
            DLOG(ERROR) << channel_name()
                << ", msg type: "
                << type()
                << ", client msg id: " 
                << parent_id_ 
                << ", to server msg id: "
                << id()
                << ", server error: invalid client symmetric key";
            //parent->Notify(true);
            break;
        case lcs::extra::Result::kTooManyConnections:
            parent->error_code_ = ErrorCode::Request();
            DLOG(ERROR) << channel_name()
                << ", msg type: "
                << type()
                << ", client msg id: " 
                << parent_id_ 
                << ", to server msg id: "
                << id()
                << ", server error: too many connections";
            //parent->Notify(true);
            break;
        case lcs::extra::Result::kRetryReq:
            parent->error_code_ = ErrorCode::Request();
            DLOG(ERROR) << channel_name()
                << ", msg type: "
                << type()
                << ", client msg id: " 
                << parent_id_ 
                << ", to server msg id: "
                << id()
                << ", server error: retry req for debug";
            break;
        default:
            LOG(ERROR) << channel_name()
                << ", msg type: "
                << type()
                << ", client msg id: " 
                << parent_id_ 
                << ", to server msg id: "
                << id()
                << ", server error: unknown error";
            parent->error_code_ = ErrorCode::Server();
            //parent->Notify(true);
            break;
    }
}

std::string ToServer::channel_name()
{
    boost::shared_ptr<ldd::net::Channel> c = channel();
    if (c != NULL) {
        return c->name();
    }

    return std::string("[unset channel]");
}

} /*gateway*/
} /*lcs*/

