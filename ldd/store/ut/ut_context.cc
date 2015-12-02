
#include <boost/shared_ptr.hpp>
#include <glog/logging.h>
#include <arpa/inet.h>
#include "ut_context.h"
#include "store_proto.h"
#include "response.h"



using namespace ldd::protocol;



namespace ldd{
namespace storage{
ReadContext::~ReadContext(){
    delete gms_;
    LOG(INFO)<<"ReadContext::~ReadContext()";
}

bool ReadContext::Init(Payload* request,
    ldd::util::TimeDiff* recv_timeout,
    ldd::util::TimeDiff* done_timeout){
    net::Buffer buf;
    int size = gms_->MaxSize();
    buf.Reset(size);
    size_t real_len = gms_->SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    LOG(INFO)<<"ReadContext::Init() make a payload len="<<real_len;
    return true;
}

bool ReadContext::Recv(const Payload& response,
    ldd::util::TimeDiff* recv_timeout){
    LOG(INFO)<<"ReadContext::Recv() get result payload len="
        <<response.body().size();
    GetOrListResponse res; 
    res.ParseFrom(response.body().ptr(), response.body().size());

    LOG(INFO)<<"ReadContext::Recv() verson="<<res.s64Version_<<" key_="
        <<res.key_.ToString()<<" value_="<<res.value_.ToString()
        <<" ttl="<<res.u64ttl_;
    return true;
}

void ReadContext::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"ReadContext::Done() result code="<<result.code();
    }else{
        LOG(ERROR)<<"ReadContext::Done() error string="<<result.ToString();
    }
}

CheckContext::~CheckContext(){
    delete gms_;
    LOG(INFO)<<"CheckContext::~CheckContext()";
}

bool CheckContext::Init(Payload* request,
    ldd::util::TimeDiff* recv_timeout,
    ldd::util::TimeDiff* done_timeout){
    net::Buffer buf;
    int size = gms_->MaxSize();
    buf.Reset(size);
    size_t real_len = gms_->SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    LOG(INFO)<<"CheckContext::Init() make a payload len="<<real_len;
    return true;
}

bool CheckContext::Recv(const Payload& response,
    ldd::util::TimeDiff* recv_timeout){
    LOG(INFO)<<"CheckContext::Recv() get result payload len="
        <<response.body().size();
    CheckResponse res;
    res.ParseFrom(response.body().ptr(), response.body().size());

    LOG(INFO)<<"CheckContext::Recv() version="<<res.s64Version_<<" key_="
        <<res.key_.ToString()
        <<" ttl="<<res.u64ttl_;
    return true;
}

void CheckContext::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"CheckContext::Done() result code="<<result.code();
    }else{
        LOG(ERROR)<<"CheckContext::Done() error string="<<result.ToString();
    }
}


MutateContext::~MutateContext(){
    delete gms_;
    LOG(INFO)<<"MutateContext::~MutateContext()";
}

bool MutateContext::Init(Payload* request,
    ldd::util::TimeDiff* recv_timeout,
    ldd::util::TimeDiff* done_timeout){
    net::Buffer buf;
    int size = gms_->MaxSize();
    buf.Reset(size);
    size_t real_len = gms_->SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    LOG(INFO)<<"MutateContext::Init() make a payload len="<<real_len;
    return true;
}

bool MutateContext::Recv(const Payload& response,
    ldd::util::TimeDiff* recv_timeout){
    LOG(INFO)<<"MutateContext::Recv() get result payload len="
        <<response.body().size();
    MutateResponse res;
    res.ParseFrom(response.body().ptr(), response.body().size());
    LOG(INFO)<<"MutateContext::Recv() version="<<res.s64Version_;
    return true;
}

void MutateContext::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"MutateContext::Done() result code="<<result.code();
    }else{
        LOG(ERROR)<<"MutateContext::Done() error string="<<result.ToString();
    }
}

ListContext::~ListContext(){
    delete gms_;
    LOG(INFO)<<"ListContext::~ListContext()";
}

bool ListContext::Init(Payload* request,
    ldd::util::TimeDiff* recv_timeout,
    ldd::util::TimeDiff* done_timeout){
    net::Buffer buf;
    int size = gms_->MaxSize();
    buf.Reset(size);
    size_t real_len = gms_->SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    LOG(INFO)<<"ListContext::Init() make a payload len="<<real_len;
    return true;
}

bool ListContext::Recv(const Payload& response,
    ldd::util::TimeDiff* recv_timeout){
    LOG(INFO)<<"ListContext::Recv() get result payload len="
        <<response.body().size();
    GetOrListResponse res; 
    res.ParseFrom(response.body().ptr(), response.body().size());

    LOG(INFO)<<"ListContext::Recv() verson="<<res.s64Version_<<" key_="
        <<res.key_.ToString()<<" value_="<<res.value_.ToString()
        <<" ttl="<<res.u64ttl_;
    *recv_timeout  = ldd::util::TimeDiff::Milliseconds(200);
    return true;
}

void ListContext::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"ListContext::Done() result code="<<result.code();
    }else{
        LOG(ERROR)<<"ListContext::Done() error string="<<result.ToString();
    }
}


AtomicIncrContext::~AtomicIncrContext(){
    delete gms_;
    LOG(INFO)<<"AtomicIncrContext::~AtomicIncrContext()";
}

bool AtomicIncrContext::Init(Payload* request,
    ldd::util::TimeDiff* recv_timeout,
    ldd::util::TimeDiff* done_timeout){
    net::Buffer buf;
    int size = gms_->MaxSize();
    buf.Reset(size);
    size_t real_len = gms_->SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    LOG(INFO)<<"AtomicIncrContext::Init() make a payload len="<<real_len;
    return true;
}

bool AtomicIncrContext::Recv(const Payload& response,
    ldd::util::TimeDiff* recv_timeout){
    LOG(INFO)<<"AtomicIncrContext::Recv() get result payload len="
        <<response.body().size();
    CasResponse res; 
    res.ParseFrom(response.body().ptr(), response.body().size());

    LOG(INFO)<<"AtomicIncrContext::Recv() verson="<<res.s64Version_<<" key_="
        <<res.key_.ToString()<<" value_="<<ntohl(*(int *)res.value_.data());
    return true;
}

void AtomicIncrContext::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"AtomicIncrContext::Done() result code="<<result.code();
    }else{
        LOG(ERROR)<<"AtomicIncrContext::Done() error string="<<result.ToString();
    }
}

AtomicAppendContext::~AtomicAppendContext(){
    delete gms_;
    LOG(INFO)<<"AtomicAppendContext::~AtomicAppendContext()";
}
bool AtomicAppendContext::Init(Payload* request,
    ldd::util::TimeDiff* recv_timeout,
    ldd::util::TimeDiff* done_timeout){
    net::Buffer buf;
    int size = gms_->MaxSize();
    buf.Reset(size);
    size_t real_len = gms_->SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    LOG(INFO)<<"AtomicAppendContext::SerializeTo() make a payload len="<<real_len;
    return true;
}

bool AtomicAppendContext::Recv(const Payload& response,
    ldd::util::TimeDiff* recv_timeout){
    LOG(INFO)<<"AtomicAppendContext::Recv() get result payload len="
        <<response.body().size();
    CasResponse res; 
    res.ParseFrom(response.body().ptr(), response.body().size());

    LOG(INFO)<<"AtomicAppendContext::Recv() verson="<<res.s64Version_<<" key_="
        <<res.key_.ToString()<<" value_="<<res.value_.ToString();
    return true;
}

void AtomicAppendContext::Done(const Result& result){
    if (result.IsOk()){
        LOG(INFO)<<"AtomicAppendContext::Done() result code="<<result.code();
    }else{
        LOG(ERROR)<<"AtomicAppendContext::Done() error string="<<result.ToString();
    }
}



}//namespace storage
}//namespace ldd


