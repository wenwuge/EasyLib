#include <glog/logging.h>

#include <ldd/net/channel.h>

#include "ldd/proxy/proxy.h"
#include "ldd/proxy/p2s/atomic_append_ex.h"
#include "ldd/proxy/c2p/atomic_append.h"
#include "ldd/proxy/proxy_type.h"

using namespace ldd::net;

C2PAtomicAppend::C2PAtomicAppend(Proxy *proxy) 
    : proxy_(proxy) 
      , namespace_id_(0)
      , hash_val_(0)
      , bucket_id_(0)
      , farm_id_(0),
      code_(kOk)
{
}


C2PAtomicAppend::~C2PAtomicAppend(){

} 

void C2PAtomicAppend::Init(const Payload& request, Action* next){
    //(1).处理请求
    LOG(INFO) << "C2PAtomicAppend::Init() recv atomic append mesasge, len=" 
                    <<request.body().size(); 
    if (!req_.ParseFrom(request.body().ptr(), request.body().size())) {
        LOG(ERROR) << "parse body for atomic append message error";
        channel()->Close();
        return ;
    }
    else {
        LOG(INFO) << "C2PAtomicAppend::Init() Parse Ok" 
            << ", namespace=" << req_.name_space_.ToString()
            << ", key=" << req_.key_.ToString()
            << ", position=" << req_.u8position_
            << ", slice=" << req_.content_.ToString()
            << ", ttl=" << req_.u64ttl_;
    }

    request_ = request;                                 

    namespace_id_ = proxy_->GetNameSpaceID(req_.name_space_.ToString());
    if (namespace_id_ < 0) {
        LOG(ERROR) << "get namespace id error";
        code_ = kNotValidNameSpace;
        *next = kDone;
        return ;
    }
    LOG(INFO) << "C2PAtomicAppend::Init() namespace id=" << namespace_id_;

    hash_val_ = proxy_->GetHashValue(req_.key_);
    bucket_id_ = proxy_->GetBucketID(hash_val_);
    if (bucket_id_ == -1) {
        LOG(ERROR) << "C2PAtomicAppend::Init() get bucket id error, key=" 
            << req_.key_.ToString();
        code_ = kNotValidBucketId;
        *next = kDone;
        return ;
    }

    farm_id_ = proxy_->GetFarmID(bucket_id_);
    if (farm_id_ == -1) {
        LOG(ERROR) << "get farm id error, key=" 
            << req_.key_.ToString();
        code_ = kNotValidFarmId;
        *next = kDone;
        return;
    }

    SendRequest();
    *next = kWait;
    return;
}

void C2PAtomicAppend::Emit(Payload* response, Action* next){
    *next = kWait;
    code_ = kOk;
    *response = response_;
    LOG(INFO)<<"C2PAtomicAppend::Emit(), id="<<id()
        <<" response size"<<response_.body().size();
}

void C2PAtomicAppend::Done(Code* code){
    *code = code_;
    LOG(INFO)<<"C2PAtomicAppend::Done() code="<<code_;
}

void C2PAtomicAppend::SendRequest(){
    LOG(INFO) << "C2PAtomicAppend::Init() send request called, farm id=" 
              << farm_id_;


    //boost::shared_ptr<P2SAtomicAppend> aa(new P2SAtomicAppend(this));
    boost::shared_ptr<P2SAtomicAppend> aa(new 
        P2SAtomicAppend(boost::static_pointer_cast<C2PAtomicAppend>(shared_from_this()), 
        proxy_->get_recv_timeout(), proxy_->get_done_timeout()));
    //p2s_atomic_append_ = aa;
    aa->req_.u16Ns_ = namespace_id_;
    aa->req_.u16BucketID_ = bucket_id_;
    aa->req_.key_ = req_.key_;
    aa->req_.u8position_ = req_.u8position_;
    aa->req_.content_ = req_.content_;
    aa->req_.u64ttl_ = req_.u64ttl_;


    // todo if node channel on this farm_id, core 
    boost::shared_ptr<Channel> channel = proxy_->GetWriteChannel(farm_id_);
    CHECK(channel.get() != NULL) << "get write channel failed";
    channel->Post(aa, ldd::util::TimeDiff::Milliseconds(200), true);
}

