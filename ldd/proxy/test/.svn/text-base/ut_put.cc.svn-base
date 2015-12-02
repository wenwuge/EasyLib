#include <map>
#include <glog/logging.h>
#include "ut_put.h"

using namespace ldd::net;

//void Put::SerializeTo(Buffer* request,
//        std::map<int8_t, Buffer>* extras)
//{
//    //构造请求，填充到request
//    request->Reset(req_.MaxSize());
//    int len = req_.SerializeTo(request->ptr());
//    request->ReSize(len);
//}
//
//void Put::HandleResult(const Buffer &result
//        , const std::map<int8_t, Buffer>& extras)
//{
//    if (!res_.ParseFrom(result.ptr(), result.len())) {
//        LOG(ERROR) << "parse error from buff to response";
//        return;
//    }
//
//    LOG(INFO) << "version=" << res_.s64Version_;
//
//    return;
//}

bool Put::Init(Payload* request,
                       ldd::util::TimeDiff* recv_timeout,
                       ldd::util::TimeDiff* done_timeout){
    ldd::net::Buffer buf;
    int size = req_.MaxSize();
    buf.Reset(size);
    size_t real_len = req_.SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    LOG(INFO)<<"Put::Init() make a payload len="<<real_len;
    return true;
}

bool Put::Recv(const Payload& response,
                       ldd::util::TimeDiff* recv_timeout){
    LOG(INFO)<<"Put::Recv() get result payload len="
       <<response.body().size();
    res_.ParseFrom(response.body().ptr(), response.body().size());

    LOG(INFO)<<"Put::Recv() verson="<<res_.s64Version_;
    return true;
}

void Put::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"Put::Done() result code="<<result.code();
    }else{
        LOG(ERROR)<<"Put::Done() error string="<<result.ToString();
    }
}


