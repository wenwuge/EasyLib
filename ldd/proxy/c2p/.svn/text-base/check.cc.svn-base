#include <glog/logging.h>
#include <ldd/net/channel.h>
#include "ldd/proxy/proxy.h"
#include "ldd/proxy/c2p/check.h"
#include "ldd/proxy/p2s/check_ex.h"
#include "ldd/proxy/proxy_type.h"

using namespace ldd::net;

C2PCheck::C2PCheck(Proxy *proxy) 
    : proxy_(proxy)
      , namespace_id_(0)
      , hash_val_(0)
      , bucket_id_(0)
      , farm_id_(0),
      code_(kOk)

{
}

C2PCheck::~C2PCheck()
{
}

void C2PCheck::Init(const Payload& request, Action* next){
    //(1).处理请求
    LOG(INFO) << "C2PCheck::Init() recv check mesasge, len=" << request.body().size(); 
    if (!req_.ParseFrom(request.body().ptr(), request.body().size())) {
        LOG(ERROR) << "parse body for check message error";
        channel()->Close();
        return ;
    }
    else {
        LOG(INFO) << "C2PCheck::Init() Parse Ok" 
            << ", namespace=" << req_.name_space_.ToString()
            << ", key len=" << req_.key_.size()
            << ", key value=" << req_.key_.ToString();
    }

    request_ = request;

    namespace_id_ = proxy_->GetNameSpaceID(req_.name_space_.ToString());
    if (namespace_id_ < 0) {
        LOG(ERROR) << "C2PCheck::Init() get namespace id error";
        code_ = kNotValidNameSpace;
        *next = kDone;
        return ;
    }
    LOG(INFO) << "C2PCheck::Init() namespace id=" << namespace_id_;

    hash_val_ = proxy_->GetHashValue(req_.key_);
    bucket_id_ = proxy_->GetBucketID(hash_val_);
    if (bucket_id_ == -1) {
        LOG(ERROR) << "get bucket id error, key=" 
            << req_.key_.ToString();
        code_ = kNotValidBucketId;
        *next = kDone;
        return ;
    }

    int farm_id_ = proxy_->GetFarmID(bucket_id_);
    if (farm_id_ == -1) {
        LOG(ERROR) << "C2PCheck::Init() get farm id error, key=" 
            << req_.key_.ToString();
        code_ = kNotValidFarmId;
        *next = kDone;
        return;
    }

    SendRequest();
    *next = kWait;
    return ;
}

void C2PCheck::SendRequest(){
    boost::shared_ptr<P2SCheck> check(new 
        P2SCheck(boost::static_pointer_cast<C2PCheck>(shared_from_this()), 
        proxy_->get_recv_timeout(), proxy_->get_done_timeout()));
    //p2s_check_ = check;
    check->req_.u16Ns_ = namespace_id_;
    check->req_.u16BucketID_ = bucket_id_;
    check->req_.key_ = req_.key_;

    //设置发送超时时间
    
    boost::shared_ptr<Channel> channel = proxy_->GetReadChannel(hash_val_
                                                                , farm_id_);
    CHECK(channel.get() != NULL) << "C2PCheck::Init() get read channel failed";
    channel->Post(check, ldd::util::TimeDiff::Milliseconds(200), true);
}

void C2PCheck::Emit(Payload* response, Action* next){
    *next = kWait;
    code_ = kOk;
    *response = response_;
    LOG(INFO)<<"C2PCheck::Emit(), id="<<id()
        <<" response size"<<response_.body().size();
}

void C2PCheck::Done(Code* code){
    *code = code_;
    LOG(INFO)<<"C2PCheck::Done() code="<<code_;
}

