#include <map>
#include <glog/logging.h>
#include "ut_list.h"

using namespace ldd::net;

//void List::SerializeTo(Buffer* request,
//        std::map<int8_t, Buffer>* extras)
//{
//    //构造请求，填充到request
//    request->Reset(req_.MaxSize());
//    int len = req_.SerializeTo(request->ptr());
//    request->ReSize(len);
//}
//
//void List::HandleResult(const Buffer &result
//        , const std::map<int8_t, Buffer>& extras)
//{
//    if (!res_.ParseFrom(result.ptr(), result.len())) {
//        LOG(ERROR) << "parse error from buff to response";
//        return;
//    }
//
//    LOG(INFO) << "key: " << res_.key_.data()
//        << ", value: " << res_.value_.data();
//
//    return;
//}

bool List::Init(Payload* request,
                       ldd::util::TimeDiff* recv_timeout,
                       ldd::util::TimeDiff* done_timeout){
    ldd::net::Buffer buf;
    int size = req_.MaxSize();
    buf.Reset(size);
    size_t real_len = req_.SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    LOG(INFO)<<"List::Init() make a payload len="<<real_len;
    return true;
}

bool List::Recv(const Payload& response,
                       ldd::util::TimeDiff* recv_timeout){
    LOG(INFO)<<"List::Recv() get result payload len="
       <<response.body().size();
    res_.ParseFrom(response.body().ptr(), response.body().size());

    LOG(INFO)<<"List::Recv() verson="<<res_.s64Version_<<" key_="
       <<res_.key_.ToString()<<" value_="<<res_.value_.ToString()
       <<" ttl="<<res_.u64ttl_;

    // this value must be set, in list interface
    *recv_timeout = ldd::util::TimeDiff::Milliseconds(200);   
    return true;
}

void List::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"List::Done() result code="<<result.code();
    }else{
        LOG(ERROR)<<"List::Done() error string="<<result.ToString();
    }
}



