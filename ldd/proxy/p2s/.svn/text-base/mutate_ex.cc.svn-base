// p2s_get.cc (2013-07-10)
// Yan Gaofeng (yangaofeng@360.cn)

#include <boost/bind.hpp>
#include <glog/logging.h>

#include "ldd/net/channel.h"

#include "ldd/proxy/p2s/mutate_ex.h"
#include "ldd/proxy/c2p/mutate.h"
#include "../proxy_type.h"


using namespace ldd::net;
using namespace ldd::protocol;

bool P2SMutate::Init(Payload* request,
                   ldd::util::TimeDiff* recv_timeout,
                   ldd::util::TimeDiff* done_timeout) {
    ldd::net::Buffer buf;
    buf.Reset(req_.MaxSize());
    int len = req_.SerializeTo(buf.ptr());
    buf.ReSize(len);

    request->SetBody(buf);

    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    *done_timeout = ldd::util::TimeDiff::Milliseconds(done_timeout_);

    DLOG(INFO) << "P2SMutate::Init() construct a mutate msg size="<<len;
    return true;
}

bool P2SMutate::Recv(const Payload& response,
                     ldd::util::TimeDiff* recv_timeout){

    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    DLOG(INFO) << "P2SMutate::Recv() p2s mutate handle result called"
              <<" address:"<<(void*)c2p_mutate_.get()
              <<" id="<<c2p_mutate_->id();

    //解析结果
    /*if (!c2p_mutate_->res_.ParseFrom(response.body().ptr(), 
                                    response.body().size())) {
       LOG(FATAL) << "parse mutate result error";
       return false;
    }*/

    c2p_mutate_->response_ = response;

    //发送结果
    c2p_mutate_->Notify();
    return true;
}

void P2SMutate::Done(const Result& result){
    // 错误的时候， 没有调用c2p的方法；todo
    if (result.IsOk()){
        DLOG(INFO)<<"P2SMutate::Done() end result code="<<result.code();
    }else if(result.IsFailed()){
        c2p_mutate_->SetCode(kProxyFailed);
        LOG(WARNING)<<"P2SMutate::Done() end result IsFailed";
    }else if(result.IsTimedOut()){
        c2p_mutate_->SetCode(kProxyTimedOut);
        LOG(WARNING)<<"P2SMutate::Done() end result IsTimedOut";
    }else if (result.IsCanceled()){
        c2p_mutate_->SetCode(kProxyCanceled);
        LOG(WARNING)<<"P2SMutate::Done() end result IsCanceled";
    }
    c2p_mutate_->Notify(true);
}


//void P2SMutate::HandleFinalize()
//{
//    LOG(INFO) << "p2s mutate handle finalized";
//    c2p_mutate_->channel()->runner()->Dispatch(
//            boost::bind(&C2PMutate::Finalize
//                , c2p_mutate_));
//}
//
//void P2SMutate::HandleTimeout()
//{
//    //应该构造一个带有错误码的响应包
//    LOG(INFO) << "p2s mutate handle time out";
//    c2p_mutate_->res_.s64Version_ = -1;
//    c2p_mutate_->channel()->runner()->Dispatch(
//            boost::bind(&C2PMutate::SendResult
//                , c2p_mutate_));
//}
//
//void P2SMutate::HandleError()
//{
//    LOG(INFO) << "p2s mutate handle error";
//    c2p_mutate_->res_.s64Version_ = -1;
//    c2p_mutate_->channel()->runner()->Dispatch(
//            boost::bind(&C2PMutate::SendResult
//                , c2p_mutate_));
//}

