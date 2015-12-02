#include <map>
#include <glog/logging.h>
#include "ut_check.h"

using namespace ldd::net;


bool Check::Init(Payload* request,
                       ldd::util::TimeDiff* recv_timeout,
                       ldd::util::TimeDiff* done_timeout){
    ldd::net::Buffer buf;
    int size = req_.MaxSize();
    buf.Reset(size);
    size_t real_len = req_.SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    LOG(INFO)<<"Check::Init() make a payload len="<<real_len;
    return true;
}

bool Check::Recv(const Payload& response,
                       ldd::util::TimeDiff* recv_timeout){
    LOG(INFO)<<"Check::Recv() get result payload len="
       <<response.body().size();
    res_.ParseFrom(response.body().ptr(), response.body().size());

    LOG(INFO)<<"Check::Recv() verson="<<res_.s64Version_<<" key_="
       <<res_.key_.ToString()
       <<" ttl="<<res_.u64ttl_;
    return true;
}

void Check::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"Check::Done() result code="<<result.code();
    }else{
        LOG(ERROR)<<"Check::Done() error string="<<result.ToString();
    }
}





