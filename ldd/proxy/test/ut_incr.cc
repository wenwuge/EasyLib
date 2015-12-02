#include <map>
#include <glog/logging.h>
#include <arpa/inet.h>
#include "ut_incr.h"

using namespace ldd::net;

//void Incr::SerializeTo(Buffer* request,
//        std::map<int8_t, Buffer>* extras)
//{
//    //构造请求，填充到request
//    request->Reset(req_.MaxSize());
//    int len = req_.SerializeTo(request->ptr());
//    request->ReSize(len);
//}
//
//void Incr::HandleResult(const Buffer &result
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
bool Incr::Init(Payload* request,
                       ldd::util::TimeDiff* recv_timeout,
                       ldd::util::TimeDiff* done_timeout){
    ldd::net::Buffer buf;
    int size = req_.MaxSize();
    buf.Reset(size);
    size_t real_len = req_.SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    LOG(INFO)<<"Incr::Init() make a payload len="<<real_len;
    return true;
}

bool Incr::Recv(const Payload& response,
                       ldd::util::TimeDiff* recv_timeout){
    LOG(INFO)<<"Incr::Recv() get result payload len="
       <<response.body().size();
    res_.ParseFrom(response.body().ptr(), response.body().size());

    LOG(INFO)<<"Incr::Recv() verson="<<res_.s64Version_<<" key_="
       <<res_.key_.ToString()<<"value="<<ntohl(*(int *)res_.value_.data());
    return true;
}

void Incr::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"Incr::Done() result code="<<result.code();
    }else{
        LOG(ERROR)<<"Incr::Done() error string="<<result.ToString();
    }
}

