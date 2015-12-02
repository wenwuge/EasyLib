#include <glog/logging.h>
#include <ldd/net/channel.h>
#include "ldd/proxy/proxy.h"
#include "ldd/proxy/c2p/atomic_incr.h"
#include "ldd/proxy/p2s/atomic_incr_ex.h"
#include "ldd/proxy/proxy_type.h"

using namespace ldd::net;

C2PAtomicIncr::C2PAtomicIncr(Proxy *proxy) 
    : proxy_(proxy)
      , namespace_id_(0)
      , hash_val_(0)
      , bucket_id_(0)
      , farm_id_(0),
      code_(kOk)
{
}

C2PAtomicIncr::~C2PAtomicIncr()
{
}

void C2PAtomicIncr::Init(const Payload& request, Action* next){
    //(1).处理请求
    LOG(INFO) << "C2PAtomicIncr::Init() recv atomic incr mesasge, len=" << request.body().size(); 
    if (!req_.ParseFrom(request.body().ptr(), request.body().size())) {
        LOG(ERROR) << "C2PAtomicIncr::Init() parse body for atomic incr message error";
        channel()->Close();
        return ;
    }
    else {
        LOG(INFO) << "C2PAtomicIncr::Init() Parse Ok" 
            << ", namespace=" << req_.name_space_.ToString()
            << ", key=" << req_.key_.ToString()
            << ", operand=" << req_.s32operand_
            << ", initial=" << req_.s32initial_
            << ", ttl=" << req_.u64ttl_;
    }

    request_ = request;

    namespace_id_ = proxy_->GetNameSpaceID(req_.name_space_.ToString());
    if (namespace_id_ < 0) {
        LOG(ERROR) << "C2PAtomicIncr::Init() get namespace id error";
        //channel()->Close();
        code_ = kNotValidNameSpace;
        *next = kDone;
        return ;
    }
    LOG(INFO) << "C2PAtomicIncr::Init() namespace id=" << namespace_id_;

    hash_val_ = proxy_->GetHashValue(req_.key_);
    bucket_id_ = proxy_->GetBucketID(hash_val_);
    if (bucket_id_ == -1) {
        LOG(ERROR) << "C2PAtomicIncr::Init() get bucket id error, key=" 
            << req_.key_.ToString();
        code_ = kNotValidBucketId;
        *next = kDone;
        //channel()->Close();
        return ;
    }

    int farm_id_ = proxy_->GetFarmID(bucket_id_);
    if (farm_id_ == -1) {
        LOG(ERROR) << "C2PAtomicIncr::Init() get farm id error, key=" 
            << req_.key_.ToString();
        code_ = kNotValidFarmId;
        *next = kDone;
        return ;
    }

    SendRequest();
    *next = kWait;
    return ;
}

void C2PAtomicIncr::SendRequest(){

    //boost::shared_ptr<P2SAtomicIncr> m(new P2SAtomicIncr(this));
    boost::shared_ptr<P2SAtomicIncr> m(new 
        P2SAtomicIncr(boost::static_pointer_cast<C2PAtomicIncr>(shared_from_this()), 
        proxy_->get_recv_timeout(), proxy_->get_done_timeout()));
    //p2s_atomic_incr_ = m;
    m->req_.u16Ns_ = namespace_id_;
    m->req_.u16BucketID_ = bucket_id_;
    m->req_.key_ = req_.key_;
    m->req_.s32operand_ = req_.s32operand_;
    m->req_.s32initial_ = req_.s32initial_;
    m->req_.u64ttl_ = req_.u64ttl_;

    //设置发送超时时间

    boost::shared_ptr<Channel> channel = proxy_->GetWriteChannel(farm_id_);
    CHECK(channel.get() != NULL) 
                << "C2PAtomicIncr::SendRequest() get write channel failed";
    channel->Post(m, ldd::util::TimeDiff::Milliseconds(200), true);
}

void C2PAtomicIncr::Emit(Payload* response, Action* next){
    *next = kWait;
    code_ = kOk;
    *response = response_;
    LOG(INFO)<<"C2PAtomicIncr::Emit(), id="<<id()
        <<" response size"<<response_.body().size();
}

void C2PAtomicIncr::Done(Code* code){
    *code = code_;
    LOG(INFO)<<"C2PAtomicIncr::Done() code="<<code_;
}



//void C2PAtomicIncr::HandleCancel()
//{
//    LOG(INFO) << "HandleCancel called";
//    p2s_atomic_incr_.Load()->Cancel();
//}

//void C2PAtomicIncr::SendResult()
//{
//    //构造响应包
//    Buffer data(res_.MaxSize());
//    int len = res_.SerializeTo(data.ptr());
//    data.ReSize(len);
//
//    //发送响应
//    /*bool ret = Respond(data);
//    if (!ret) {
//        LOG(ERROR) << "send atomic increase response to client failed";
//    }*/
//}


