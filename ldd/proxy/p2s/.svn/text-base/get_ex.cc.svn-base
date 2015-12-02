// p2s_get.cc (2013-07-10)
// Yan Gaofeng (yangaofeng@360.cn)

#include <boost/bind.hpp>
#include <glog/logging.h>
#include "ldd/util/atomic.h"
#include "ldd/net/channel.h"

#include "ldd/proxy/p2s/get_ex.h"
#include "../proxy_type.h"

using namespace ldd::net;
using namespace ldd::protocol;


bool P2SGet::Init(Payload* request,
                    ldd::util::TimeDiff* recv_timeout,
                    ldd::util::TimeDiff* done_timeout) {
    ldd::net::Buffer buf;
    buf.Reset(req_.MaxSize());
    int len = req_.SerializeTo(buf.ptr());
    buf.ReSize(len);

    request->SetBody(buf);

    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    *done_timeout = ldd::util::TimeDiff::Milliseconds(done_timeout_);

    DLOG(INFO) << "P2SGet::Init() construct a atomic append msg size="<<len;
    return true;

}

bool P2SGet::Recv(const Payload& response,
                    ldd::util::TimeDiff* recv_timeout){
    DLOG(INFO) << "P2SGet::Recv() p2s get handle result called";
    //解析结果
    /*if (!c2p_get_->res_.ParseFrom(response.body().ptr(), 
                                    response.body().size())) {
        LOG(FATAL) << "P2SGet::Recv() parse get result error";
        return false;
    }*/

    c2p_get_->response_ = response;
    //PrintResult();

    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);

    c2p_get_->Notify();
    return true;
}

void P2SGet::Done(const Result& result){
    if (result.IsOk()){
        DLOG(INFO)<<"P2SGet::Done() result code="<<result.code();
    }else if(result.IsFailed()){
         c2p_get_->SetCode(kProxyFailed);
         LOG(WARNING)<<"P2SGet::Done() result IsFailed";
    }else if(result.IsTimedOut()){
         c2p_get_->SetCode(kProxyTimedOut);
         LOG(WARNING)<<"P2SGet::Done() result IsTimedOut";
    }else if (result.IsCanceled()){
         c2p_get_->SetCode(kProxyCanceled);
         LOG(WARNING)<<"P2SGet::Done() result IsCanceled";
    }
    c2p_get_->Notify(true);
}   

//void P2SGet::HandleFinalize()
//{
//    LOG(INFO) << "p2s get finalized";
//    c2p_get_->p2s_get_.Store(NULL, ldd::util::MemoryOrder::kRelease);
//    c2p_get_->channel()->runner()->Dispatch(
//            boost::bind(&C2PGet::Finalize
//                , c2p_get_));
//}
//
//void P2SGet::HandleTimeout()
//{
//    //应该构造一个带有错误码的响应包
//    LOG(INFO) << "p2s get time out";
//    c2p_get_->p2s_get_.Store(NULL, ldd::util::MemoryOrder::kRelease);
//    c2p_get_->res_.s64Version_ = -1;
//    c2p_get_->channel()->runner()->Dispatch(
//            boost::bind(&C2PGet::SendResult
//                , c2p_get_
//                , Buffer()));
//}
//
//void P2SGet::HandleError()
//{
//    LOG(INFO) << "p2s get error";
//    c2p_get_->p2s_get_.Store(NULL, ldd::util::MemoryOrder::kRelease);
//    c2p_get_->res_.s64Version_ = -1;
//    c2p_get_->channel()->runner()->Dispatch(
//            boost::bind(&C2PGet::SendResult
//                , c2p_get_
//                , Buffer()));
//}

void P2SGet::PrintResult()
{
    if (c2p_get_->res_.s64Version_ < 0) {
        LOG(ERROR) << "P2SGet::PrintResult() get key("<< c2p_get_->res_.key_.ToString() 
            << ") response is null, version=" << c2p_get_->res_.s64Version_;
    }
    else {
        DLOG(INFO) << "P2SGet::PrintResult() result: "
            "version=" << c2p_get_->res_.s64Version_
            << ", key_=" << c2p_get_->res_.key_.ToString()
            << ", value.size=" << c2p_get_->res_.value_.size()
            << ", ttl=" << c2p_get_->res_.u64ttl_;
    }
}

