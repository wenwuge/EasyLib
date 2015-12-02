// p2s_get.cc (2013-07-10)
// Yan Gaofeng (yangaofeng@360.cn)

#include <glog/logging.h>

#include <boost/bind.hpp>
#include "ldd/net/channel.h"

#include "ldd/proxy/c2p/list.h"
#include "ldd/proxy/p2s/list_ex.h"
#include "../proxy_type.h"


using namespace ldd::net;
using namespace ldd::protocol;

bool P2SList::Init(Payload* request,
                  ldd::util::TimeDiff* recv_timeout,
                  ldd::util::TimeDiff* done_timeout) {
    ldd::net::Buffer buf;
    buf.Reset(req_.MaxSize());
    int len = req_.SerializeTo(buf.ptr());
    buf.ReSize(len);

    request->SetBody(buf);

    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    *done_timeout = ldd::util::TimeDiff::Milliseconds(done_timeout_);

    LOG(INFO) << "P2SList::Init() construct a atomic append msg size="<<len;
    return true;
}

bool P2SList::Recv(const Payload& response,
                  ldd::util::TimeDiff* recv_timeout){
    /*if (IsCanceled()) {
      CompleteRequest();
      return false;
    }*/

    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);

    ldd::protocol::GetOrListResponse *res = 
                        new ldd::protocol::GetOrListResponse();
    //解析结果
    if (!res->ParseFrom(response.body().ptr(), response.body().size())) {
      delete res;
      LOG(FATAL) << "P2SList::Recv() parse list result error";
      return false;
    }

    //发送结果
    //c2p_list_->SendResult(res, result, index_);
    c2p_list_->SendResult(res, response, index_);

    /*c2p_list_->channel()->runner()->Dispatch(
            boost::bind(&C2PList::SendResult, c2p_list_, res, result, index_));*/
    return true;
}

void P2SList::CompleteRequest(){
    LOG(INFO) << "P2SList::CompleteRequest() complete on request";

    ldd::util::MutexLock lock(&c2p_list_->mutex_);
    c2p_list_->p2s_list_info_[index_].complete = true;
    c2p_list_->p2s_list_info_[index_].p2s_list.reset();// = NULL;

    int count = c2p_list_->p2s_list_info_.size();
    bool all_complete = true;
    for (int i = 0; i < count; i++) {
        if (!c2p_list_->p2s_list_info_[i].complete) {
            all_complete = false;
        }
    }

    c2p_list_->Complete();


    /*if (all_complete) {
        LOG(INFO) << "all request is completed";
        c2p_list_->channel()->runner()->Dispatch(
                    boost::bind(&C2PList::Complete
                    , c2p_list_));
                    return;
    }*/
}

void P2SList::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"P2SList::Done() result code="<<result.code();
    }else if(result.IsFailed()){
        c2p_list_->SetCode(kProxyFailed);
        LOG(WARNING)<<"P2SList::Done() result IsFailed";
    }else if(result.IsTimedOut()){
        c2p_list_->SetCode(kProxyTimedOut);
        LOG(WARNING)<<"P2SList::Done() result IsTimedOut";
    }else if (result.IsCanceled()){
        c2p_list_->SetCode(kProxyCanceled);
        LOG(WARNING)<<"P2SList::Done() result IsCanceled";
    }
    c2p_list_->Notify(true);



    CompleteRequest();
}


//void P2SList::HandleFinalize()
//{
//    LOG(INFO) << "finalized";
//    CompleteRequest();
//
//#if 0
//    c2p_list_->channel()->runner()->Dispatch(
//            boost::bind(&C2PList::Finalize
//                , c2p_list_));
//#endif
//}

//void P2SList::HandleTimeout()
//{
//    LOG(INFO) << "time out";
//    CompleteRequest();
//#if 0
//    ldd::protocol::GetOrListResponse *res 
//        = new ldd::protocol::GetOrListResponse();
//    res->s64Version_ = -1;
//    ldd::net::Buffer result;
//    c2p_list_->channel()->runner()->Dispatch(
//            boost::bind(&C2PList::SendResult
//                , c2p_list_
//                , res
//                , result));
//#endif
//}
//
//void P2SList::HandleError()
//{
//    LOG(INFO) << "error";
//    CompleteRequest();
//
//#if 0
//    ldd::protocol::GetOrListResponse *res 
//        = new ldd::protocol::GetOrListResponse();
//    res->s64Version_ = -1;
//    ldd::net::Buffer result;
//    c2p_list_->channel()->runner()->Dispatch(
//            boost::bind(&C2PList::SendResult
//                , c2p_list_
//                , res
//                , result));
//#endif
//
//}



