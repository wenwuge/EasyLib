// p2s_atomic_incr.cc (2013-07-10)
// Yan Gaofeng (yangaofeng@360.cn)

#include <arpa/inet.h>
#include <glog/logging.h>

#include <boost/bind.hpp>
#include "ldd/net/channel.h"

#include "ldd/proxy/p2s/atomic_incr_ex.h"
#include "ldd/proxy/c2p/atomic_incr.h"
#include "../proxy_type.h"



using namespace ldd::net;
using namespace ldd::protocol;


bool P2SAtomicIncr::Init(Payload* request,
                           ldd::util::TimeDiff* recv_timeout,
                           ldd::util::TimeDiff* done_timeout) {
    //1、构造请求，填充到request和extras(如果有的话)
    ldd::net::Buffer buf;
    buf.Reset(req_.MaxSize());
    int len = req_.SerializeTo(buf.ptr());
    buf.ReSize(len);

    request->SetBody(buf);

    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    *done_timeout = ldd::util::TimeDiff::Milliseconds(done_timeout_);

    LOG(INFO) << "P2SAtomicIncr::Init() construct a atomic append msg size="<<len;
    return true;
}

bool P2SAtomicIncr::Recv(const Payload& response,
                           ldd::util::TimeDiff* recv_timeout){
                               //

    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    if (!c2p_atomic_incr_->res_.ParseFrom(response.body().ptr(), 
        response.body().size())) {
       LOG(FATAL) << "P2SAtomicIncr::Recv() parse atomic incr response error";
       return false;
    }

    c2p_atomic_incr_->response_ = response;

    LOG(INFO) << "P2SAtomicIncr::Recv() Parse ok"
       << ", version=" << c2p_atomic_incr_->res_.s64Version_
       << ", key=" << c2p_atomic_incr_->res_.key_.ToString().c_str()
       << ", value=" << ntohl(*((int *)c2p_atomic_incr_->res_.value_.data()));

    c2p_atomic_incr_->Notify();
    return true;
}

void P2SAtomicIncr::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"P2SAtomicIncr::Done() result code="<<result.code();
    }else if(result.IsFailed()){
        c2p_atomic_incr_->SetCode(kProxyFailed);
        LOG(WARNING)<<"P2SAtomicIncr::Done() result IsFailed";
    }else if(result.IsTimedOut()){
        c2p_atomic_incr_->SetCode(kProxyTimedOut);
        LOG(WARNING)<<"P2SAtomicIncr::Done() result IsTimedOut";
    }else if (result.IsCanceled()){
        c2p_atomic_incr_->SetCode(kProxyCanceled);
        LOG(WARNING)<<"P2SAtomicIncr::Done() result IsCanceled";
    }
    c2p_atomic_incr_->Notify(true);
}

//void P2SAtomicIncr::HandleFinalize()
//{
//    LOG(INFO) << "finalized";
//    c2p_atomic_incr_->channel()->runner()->Dispatch(
//            boost::bind(&C2PAtomicIncr::Finalize
//                , c2p_atomic_incr_));
//}
//
//void P2SAtomicIncr::HandleTimeout()
//{
//    LOG(INFO) << "time out";
//    c2p_atomic_incr_->res_.s64Version_ = -1;
//    c2p_atomic_incr_->channel()->runner()->Dispatch(
//            boost::bind(&C2PAtomicIncr::SendResult
//                , c2p_atomic_incr_));
//}
//
//void P2SAtomicIncr::HandleError()
//{
//    LOG(INFO) << "error";
//    c2p_atomic_incr_->res_.s64Version_ = -1;
//    c2p_atomic_incr_->channel()->runner()->Dispatch(
//            boost::bind(&C2PAtomicIncr::SendResult
//                , c2p_atomic_incr_));
//}

