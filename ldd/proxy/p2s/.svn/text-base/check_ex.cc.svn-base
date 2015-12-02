// p2s_get.cc (2013-07-10)
// Yan Gaofeng (yangaofeng@360.cn)

#include <glog/logging.h>

#include <boost/bind.hpp>
#include "ldd/net/channel.h"

#include "ldd/proxy/c2p/check.h"
#include "ldd/proxy/p2s/check_ex.h"
#include "../proxy_type.h"



using namespace ldd::net;
using namespace ldd::protocol;


bool P2SCheck::Init(Payload* request,
                         ldd::util::TimeDiff* recv_timeout,
                         ldd::util::TimeDiff* done_timeout) {
    ldd::net::Buffer buf;
    buf.Reset(req_.MaxSize());
    int len = req_.SerializeTo(buf.ptr());
    buf.ReSize(len);

    request->SetBody(buf);

    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    *done_timeout = ldd::util::TimeDiff::Milliseconds(done_timeout_);

    LOG(INFO) << "P2SCheck::Init() construct a atomic append msg size="<<len;
    return true;
}

bool P2SCheck::Recv(const Payload& response,
                         ldd::util::TimeDiff* recv_timeout){
    //解析结果
    if (!c2p_check_->res_.ParseFrom(response.body().ptr(), 
                                    response.body().size())) {
     LOG(FATAL) << "P2SCheck::Init() ParseFrom error";
     return false;
    }

    c2p_check_->response_ = response;

    c2p_check_->Notify();
    return true;
}

void P2SCheck::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"P2SCheck::Done() result code="<<result.code();
    }else if(result.IsFailed()){
        c2p_check_->SetCode(kProxyFailed);
        LOG(WARNING)<<"P2SCheck::Done() result IsFailed";
    }else if(result.IsTimedOut()){
        c2p_check_->SetCode(kProxyTimedOut);
        LOG(WARNING)<<"P2SCheck::Done() result IsTimedOut";
    }else if (result.IsCanceled()){
        c2p_check_->SetCode(kProxyCanceled);
        LOG(WARNING)<<"P2SCheck::Done() result IsCanceled";
    }
    c2p_check_->Notify(true);
}


//void P2SCheck::HandleFinalize()
//{
//    LOG(INFO) << "finalized";
//    c2p_check_->p2s_check_.Store(NULL, ldd::util::MemoryOrder::kRelease);
//    c2p_check_->channel()->runner()->Dispatch(
//            boost::bind(&C2PCheck::Finalize
//                , c2p_check_));
//}
//
//void P2SCheck::HandleTimeout()
//{
//    LOG(INFO) << "time out";
//    c2p_check_->p2s_check_.Store(NULL, ldd::util::MemoryOrder::kRelease);
//    c2p_check_->res_.s64Version_ = -1;
//    c2p_check_->channel()->runner()->Dispatch(
//            boost::bind(&C2PCheck::SendResult
//                , c2p_check_));
//}
//
//void P2SCheck::HandleError()
//{
//    LOG(INFO) << "error";
//    c2p_check_->p2s_check_.Store(NULL, ldd::util::MemoryOrder::kRelease);
//    c2p_check_->res_.s64Version_ = -1;
//    c2p_check_->channel()->runner()->Dispatch(
//            boost::bind(&C2PCheck::SendResult
//                , c2p_check_));
//}

