#include <map>
#include <glog/logging.h>
#include "ut_get.h"

using namespace ldd::net;

//void Get::SerializeTo(Buffer* request,
//        std::map<int8_t, Buffer>* extras)
//{
//    //构造请求，填充到request
//    request->Reset(req_.MaxSize());
//    int len = req_.SerializeTo(request->ptr());
//    request->ReSize(len);
//}
//
//void Get::HandleResult(const Buffer &result
//        , const std::map<int8_t, Buffer>& extras)
//{
//    if (!res_.ParseFrom(result.ptr(), result.len())) {
//        LOG(ERROR) << "parse error from buff to response";
//        return;
//    }
//
//    LOG(INFO) << "key: " << res_.key_.ToString()
//        << ", value: " << res_.value_.ToString();
//
//    return;
//}

bool Get::Init(Payload* request,
                       ldd::util::TimeDiff* recv_timeout,
                       ldd::util::TimeDiff* done_timeout){
    ldd::net::Buffer buf;
    int size = req_.MaxSize();
    buf.Reset(size);
    size_t real_len = req_.SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    LOG(INFO)<<"Get::Init() make a payload len="<<real_len;
    return true;
}

bool Get::Recv(const Payload& response,
                       ldd::util::TimeDiff* recv_timeout){
    LOG(INFO)<<"Get::Recv() get result payload len="
       <<response.body().size();
    res_.ParseFrom(response.body().ptr(), response.body().size());

    LOG(INFO)<<"Get::Recv() verson="<<res_.s64Version_<<" key_="
       <<res_.key_.ToString()<<" value_="<<res_.value_.ToString()
       <<" ttl="<<res_.u64ttl_;
    return true;
}

void Get::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"Get::Done() result code="<<result.code();
    }else{
        LOG(ERROR)<<"S::Done() error string="<<result.ToString();
    }
}



