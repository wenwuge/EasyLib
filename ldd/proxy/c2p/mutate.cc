#include <glog/logging.h>
#include <ldd/net/channel.h>
#include "ldd/proxy/proxy.h"
#include "ldd/proxy/c2p/mutate.h"
#include "ldd/proxy/p2s/mutate_ex.h"
#include "ldd/proxy/proxy_type.h"

using namespace ldd::net;

C2PMutate::C2PMutate(Proxy *proxy) 
    : proxy_(proxy)
      , namespace_id_(0)
      , hash_val_(0)
      , bucket_id_(0)
      , farm_id_(0),
      code_(kOk){
}

C2PMutate::~C2PMutate(){
    DLOG(INFO)<<"C2PMutate::~C2PMutate()"<<" id="<<id()<<(void*)this;
}

void C2PMutate::Init(const Payload& request, Action* next){
    //1.处理请求
    DLOG(INFO) << "start C2PMutate::Init() recv mutate mesasge, len=" 
               << request.body().size()<<" id="<<id()<<(void*)this; 
    if (!req_.ParseFrom(request.body().ptr(), request.body().size())) {
        LOG(ERROR) << "C2PMutate::Init() parse body fo mutate message error";
        channel()->Close();
        return ;
    }

    request_ = request;
    namespace_id_ = proxy_->GetNameSpaceID(req_.name_space_.ToString());
    if (namespace_id_ < 0) {
        LOG(ERROR) << "C2PMutate::Init() get namespace id error";
        code_ = kNotValidNameSpace;
        *next = kDone;
        return ;
    }
    DLOG(INFO) << "C2PMutate::Init() namespace id=" << namespace_id_
                    <<" key="<<req_.mutation_.key.ToString();

    hash_val_ = proxy_->GetHashValue(req_.mutation_.key);
    bucket_id_ = proxy_->GetBucketID(hash_val_);
    if (bucket_id_ == -1) {
        LOG(ERROR) << "C2PMutate::Init() get bucket id error, key=" 
            << req_.mutation_.key.ToString();
        code_ = kNotValidBucketId;
        *next = kDone;
        return ;
    }

    farm_id_ = proxy_->GetFarmID(bucket_id_);
    if (farm_id_ == -1) {
        LOG(ERROR) << "C2PMutate::Init() get farm id error, key=" 
            << req_.mutation_.key.ToString();
        
        code_ = kNotValidFarmId;
        *next = kDone;
        return;
    }

    SendRequest();
    *next = kWait;

    return ;
}

void C2PMutate::SendRequest(){
    // boost::shared_ptr<P2SMutate> mutate(new P2SMutate(this));
    boost::shared_ptr<P2SMutate> mutate(new 
        P2SMutate(boost::static_pointer_cast<C2PMutate>(shared_from_this()), 
        proxy_->get_recv_timeout(), proxy_->get_done_timeout()));
    //p2s_mutate_ = mutate;
    mutate->req_.u16Ns_ = namespace_id_;
    mutate->req_.u16BucketID_ = bucket_id_;
    mutate->req_.mutation_ = req_.mutation_;

    //设置发送超时时间

    boost::shared_ptr<Channel> channel = proxy_->GetWriteChannel(farm_id_);
    CHECK(channel.get() != NULL) << "C2PMutate::SendRequest()get read channel failed";
    channel->Post(mutate, ldd::util::TimeDiff::Milliseconds(200), true);
}


void C2PMutate::Emit(Payload* response, Action* next){
    *next = kWait;
    code_ = kOk;
    *response = response_;
    DLOG(INFO)<<"C2PMutate::Emit(), id="<<id()
        <<" response size"<<response_.body().size()
        <<" key="<<req_.mutation_.key.ToString();
}

void C2PMutate::Done(Code* code){
    *code = code_;
    DLOG(INFO)<<"C2PMutate::Done() code="<<code_<<" id="<<id()
        <<" key="<<req_.mutation_.key.ToString();
}

void C2PMutate::Cancel(){
    DLOG(INFO) << "HandleCancel called id="<<id();
}


//void C2PMutate::HandleCancel()
//{
//    LOG(INFO) << "HandleCancel called";
//    if (p2s_mutate_ != NULL) {
//        p2s_mutate_.Load()->Cancel();
//    }
//}

//void C2PMutate::SendResult()
//{
//    //构造响应包
//    Buffer data(res_.MaxSize());
//    int len = res_.SerializeTo(data.ptr());
//    data.ReSize(len);
//
//    ////发送响应
//    //bool ret = Respond(data);
//    //if (!ret) {
//    //    LOG(ERROR) << "send mutate response to client failed";
//    //}
//}



