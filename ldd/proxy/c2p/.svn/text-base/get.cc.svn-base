#include <glog/logging.h>

#include <ldd/net/channel.h>
#include "ldd/protocol/server/store_proto.h"
#include "ldd/proxy/proxy.h"
#include "ldd/proxy/c2p/get.h"
#include "ldd/proxy/p2s/get_ex.h"
#include "ldd/proxy/proxy_type.h"

using namespace ldd::net;

C2PGet::C2PGet(Proxy *proxy) 
            : proxy_(proxy)
              , hash_val_(0)
              , namespace_id_(0)
              , bucket_id_(0)
              , farm_id_(0),
              code_(kOk)
{
}

C2PGet::~C2PGet()
{
}

void C2PGet::Init(const Payload& request, Action* next){
    //1,解析请求
    DLOG(INFO) << "C2PGet::Init() c2p recv get mesasge, len=" << request.body().size(); 
    if (!req_.ParseFrom(request.body().ptr(), request.body().size())) {
        LOG(ERROR) << "C2PGet::Init() c2p parse body fo get message error";
        channel()->Close();
        return ;
    }

    request_ = request; 

    DLOG(INFO) << "C2PGet::Init() c2p Parse Ok" 
        << ", namespace=" << req_.name_space_.ToString()
        << ", key=" << req_.key_.ToString();

    namespace_id_ = proxy_->GetNameSpaceID(req_.name_space_.ToString());
    if (namespace_id_ < 0) {
        LOG(ERROR) << "C2PGet::Init() c2p get namespace id error";
        code_ = kNotValidNameSpace;
        *next = kDone;
        return ;
    }
    DLOG(INFO) << "C2PGet::Init() c2p namespace id=" << namespace_id_;

    hash_val_ = proxy_->GetHashValue(req_.key_);
    bucket_id_ = proxy_->GetBucketID(hash_val_);
    if (bucket_id_ == -1) {
        LOG(ERROR) << "C2PGet::Init() c2p get bucket id error, key=" 
            << req_.key_.ToString();
        code_ = kNotValidBucketId;
        *next = kDone;
        return ;
    }

    farm_id_ = proxy_->GetFarmID(bucket_id_);
    if (farm_id_ == -1) {
        LOG(ERROR) << "C2PGet::Init() c2p get farm id error, key=" 
            << req_.key_.ToString();
        code_ = kNotValidFarmId;
        *next = kDone;
        return ;
    }

    /*if (farm_id_ == 0 || farm_id_ == 1){
        DLOG(ERROR) << "C2PGet::Init() c2p Parse Ok key=" << req_.key_.ToString()
                <<" hash_val_="<<hash_val_;
    }*/
    

    //2,发送请求
    SendRequest();
    *next = kWait;

    return ;
}

//to server
void C2PGet::SendRequest()
{
    DLOG(INFO) << "C2PGet::SendRequest() c2p get send reqeust called";
    boost::shared_ptr<P2SGet> get(new 
        P2SGet(boost::static_pointer_cast<C2PGet>(shared_from_this()), 
        proxy_->get_recv_timeout(), proxy_->get_done_timeout()));
    //p2s_get_ = get;
    get->req_.u16Ns_ = namespace_id_;
    get->req_.u16BucketID_ = bucket_id_;
    get->req_.key_ = req_.key_;

    DLOG(INFO) << "C2PGet::SendRequest() c2p get key=" << req_.key_.ToString();

    //设置发送超时时间
    //get->set_conn_timeout(1000);
    //get->set_send_timeout(1000);
    //get->set_recv_timeout(2000);

    boost::shared_ptr<Channel> channel = proxy_->GetReadChannel(hash_val_
        , farm_id_);
    CHECK(channel.get() != NULL) << "C2PGet::SendRequest() get read channel failed";
    channel->Post(get, ldd::util::TimeDiff::Milliseconds(200), true);
}

void C2PGet::PrintResult(){
    if (res_.s64Version_ < 0) {
        LOG(ERROR) << "C2PGet::PrintResult()c2p get key("<< req_.key_.ToString() 
            << ") response is null, version=" << res_.s64Version_;
    }
    else {
        DLOG(INFO) << "C2PGet::PrintResult()c2p Get result: "
            "version=" << res_.s64Version_
            << ", key_=" << res_.key_.ToString()
            << ", value=" << atoi(res_.value_.ToString().c_str())
            << ", ttl=" << res_.u64ttl_;
    }
}

void C2PGet::Emit(Payload* response, Action* next){
    *next = kWait;
    code_ = kOk;
    *response = response_;
    DLOG(INFO)<<"C2PGet::Emit(), id="<<id()
        <<" response size"<<response_.body().size()
        <<" key=" << req_.key_.ToString();
}

void C2PGet::Done(Code* code){
    *code = code_;
    DLOG(INFO)<<"C2PGet::Done() code="<<code_
        <<" key=" << req_.key_.ToString();
}

//void C2PGet::HandleCancel()
//{
//    LOG(INFO) << "c2p cancel called";
//    //note: need rewrite
//    if (p2s_get_ != NULL) {
//        p2s_get_.Load()->Cancel();
//    }
//}

//to client
//void C2PGet::SendResult(const Buffer &buffer)
//{
//    LOG(INFO) << "c2p get send result called";
//
//    request_ = buffer;
//
//    //构造响应包
//    Buffer data(res_.MaxSize());
//    int len = res_.SerializeTo(data.ptr());
//    data.ReSize(len);
//
//    LOG(INFO) << "c2p get send result, res_.key=" << res_.key_.ToString();
//
//    PrintResult();
//
//    //发送响应
//    //bool ret = Respond(data);
//    //if (!ret) {
//    //    LOG(ERROR) << "c2p send response to client failed: "
//    //        "request is cancelled";
//    //}
//
//    if (res_.s64Version_ < 0) {
//        LOG(INFO) << "c2p get error: " << res_.s64Version_;
//    }
//}



