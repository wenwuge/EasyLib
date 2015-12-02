#include <map>
#include <glog/logging.h>
#include "ut_append.h"

using namespace ldd::net;

//void Append::SerializeTo(Buffer* request,
//        std::map<int8_t, Buffer>* extras)
//{
//    //构造请求，填充到request
//    request->Reset(req_.MaxSize());
//    int len = req_.SerializeTo(request->ptr());
//    request->ReSize(len);
//}

//void Append::HandleResult(const Buffer &result
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

bool Append::Init(Payload* request,
                       ldd::util::TimeDiff* recv_timeout,
                       ldd::util::TimeDiff* done_timeout){
    ldd::net::Buffer buf;
    int size = req_.MaxSize();
    buf.Reset(size);
    size_t real_len = req_.SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    LOG(INFO)<<"Append::Init() make a payload len="<<real_len;
    return true;
}

bool Append::Recv(const Payload& response,
                       ldd::util::TimeDiff* recv_timeout){
    LOG(INFO)<<"Append::Recv() get result payload len="
       <<response.body().size();
    res_.ParseFrom(response.body().ptr(), response.body().size());

    LOG(INFO)<<"Append::Recv() version="<<res_.s64Version_<<" key_="
       <<res_.key_.ToString()<<" value_="<<res_.value_.ToString();
    return true;
}

void Append::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"Append::Done() result code="<<result.code();
    }else{
        LOG(ERROR)<<"Append::Done() error string="<<result.ToString();
    }
}




