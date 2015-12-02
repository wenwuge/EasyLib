// p2s_atomic_append.cc (2013-07-10)
// Yan Gaofeng (yangaofeng@360.cn)

#include <glog/logging.h>

#include <boost/bind.hpp>
#include "ldd/net/channel.h"

#include "ldd/proxy/c2p/atomic_append.h"
#include "ldd/proxy/p2s/atomic_append_ex.h"
#include "../proxy_type.h"

using namespace ldd::net;
using namespace ldd::protocol;


bool P2SAtomicAppend::Init(Payload* request,
          ldd::util::TimeDiff* recv_timeout,
          ldd::util::TimeDiff* done_timeout) {
    //1、构造请求，填充到request和extras(如果有的话
    ldd::net::Buffer buf;
    buf.Reset(req_.MaxSize());
    int len = req_.SerializeTo(buf.ptr());
    buf.ReSize(len);

    request->SetBody(buf);

    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    *done_timeout = ldd::util::TimeDiff::Milliseconds(done_timeout_);

    LOG(INFO) << "P2SAtomicAppend::Init() construct a atomic append msg size="<<len;
    return true;
}

bool P2SAtomicAppend::Recv(const Payload& response,
          ldd::util::TimeDiff* recv_timeout){

    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    //解析结果
    if (!c2p_atomic_append_->res_.ParseFrom(response.body().ptr(), 
                                            response.body().size())) {
        LOG(FATAL) << "P2SAtomicAppend::Recv() parse atomic incr response error";
        return false;
    }

    c2p_atomic_append_->response_ = response;

    LOG(INFO) << "Parse ok"
      << ", version=" << c2p_atomic_append_->res_.s64Version_
      << ", key=" << c2p_atomic_append_->res_.key_.ToString()
      << ", value=" << c2p_atomic_append_->res_.value_.ToString();
    c2p_atomic_append_->Notify();

    return true;

}

void P2SAtomicAppend::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"P2SAtomicAppend::Done() result code="<<result.code();
    }else if(result.IsFailed()){
        c2p_atomic_append_->SetCode(kProxyFailed);
        LOG(WARNING)<<"P2SAtomicAppend::Done() result IsFailed";
    }else if(result.IsTimedOut()){
        c2p_atomic_append_->SetCode(kProxyTimedOut);
        LOG(WARNING)<<"P2SAtomicAppend::Done() result IsTimedOut";
    }else if (result.IsCanceled()){
        c2p_atomic_append_->SetCode(kProxyCanceled);
        LOG(WARNING)<<"P2SAtomicAppend::Done() result IsCanceled";
    }
    c2p_atomic_append_->Notify(true);

}

//void P2SAtomicAppend::HandleFinalize()
//{
//    LOG(INFO) << "finalized";
//    c2p_atomic_append_->channel()->runner()->Dispatch(
//            boost::bind(&C2PAtomicAppend::Finalize
//                , c2p_atomic_append_));
//}
//
//void P2SAtomicAppend::HandleTimeout()
//{
//    LOG(INFO) << "time out";
//    c2p_atomic_append_->res_.s64Version_ = -1;
//    c2p_atomic_append_->channel()->runner()->Dispatch(
//            boost::bind(&C2PAtomicAppend::SendResult
//                , c2p_atomic_append_));
//}
//
//void P2SAtomicAppend::HandleError()
//{
//    LOG(INFO) << "error";
//    c2p_atomic_append_->res_.s64Version_ = -1;
//    c2p_atomic_append_->channel()->runner()->Dispatch(
//            boost::bind(&C2PAtomicAppend::SendResult
//                , c2p_atomic_append_));
//}

