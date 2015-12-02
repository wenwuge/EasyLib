#include <map>
#include <glog/logging.h>
#include "ldd/util/time.h"
#include "ldd/protocol/server/store_error.h"
#include "list.h"

#include "../redo_simple_policy.h"


namespace ldd { 
namespace client {
namespace raw {

using namespace ldd::net;

ListRequest::ListRequest(ClientImpl* ci, const std::string& ns, 
    const std::string& key,
	uint32_t u32limit, uint8_t	u8position,  
	const DataCompletion& completion, 
	const std::string& using_host,
	int					using_port,
	RedoPolicy* rp) : 
	client_(ci),
	ns_(ns),
	key_(key),
	u32limit_(u32limit),
	u8position_(u8position),
	completion_(completion),
	using_host_(using_host),
	using_port_(using_port),
	rp_(rp){
}



bool ListRequest::Init(Payload* request,
    ldd::util::TimeDiff* recv_timeout,
    ldd::util::TimeDiff* done_timeout){
        protocol::ListMessage req_;
    req_.name_space_	= ns_;
    req_.key_			= key_;
    req_.u32limit_		= u32limit_;
    req_.u8position_	= u8position_;  

    *recv_timeout = util::TimeDiff::Milliseconds(client_->get_recv_timeout());
    *done_timeout = util::TimeDiff::Milliseconds(client_->get_done_timeout());

    ldd::net::Buffer buf;

    //构造请求，填充到request
    buf.Reset(req_.MaxSize());
    int len = req_.SerializeTo(buf.ptr());
    buf.ReSize(len);
    LOG(INFO)<<"ListRequest::SerializeTo() create buffer len="<<len;
    return true;
}

bool ListRequest::Recv(const Payload& response,
    ldd::util::TimeDiff* recv_timeout){

    *recv_timeout = util::TimeDiff::Milliseconds(client_->get_recv_timeout());

    if (!response_.ParseFrom(response.body().ptr(), response.body().size())) {
        LOG(ERROR) << " ListRequest::HandleResult（） parse error from buff to response";
        completion_(ldd::protocol::kNotValidPacket,"", 0);
        return false;
    }

    LOG(INFO) << "ListRequest::Recv() key: " << response_.key_.data()
        << ", value: " << response_.value_.data();
    completion_(response_.s64Version_, response_.value_.ToString(), response_.u64ttl_);
    return true;
}

void ListRequest::Done(const Result& result){
    LOG(INFO)<<"ListRequest::Done() result="<<result.code();

    if (result.IsTimedOut()){
	    if (rp_->ShouldRety(util::Time::CurrentMilliseconds())){
		    client_->List(ns_, key_, u32limit_, u8position_, 
						    completion_, using_host_,  using_port_, rp_);
	    }else{
            completion_(ldd::protocol::kTimeout, "", 0);
		    delete rp_;
		    LOG(ERROR) << "ListRequest::HandleError() timeout called";
	    }
    }else if (result.IsFailed()){
        completion_(kFailed, "", 0);
        delete rp_;
        LOG(ERROR) << "MutateRequest::Done() code="<<kFailed;
    }else if (result.IsCanceled()){
        completion_(kCancel, "", 0);
        delete rp_;
        LOG(ERROR) << "MutateRequest::Done() code="<<kCancel;
    }else if (result.IsOk()){
        delete rp_;
        if (result.code() == kOK){
            LOG(INFO)<<"MutateRequest::Done OK!!!";
        }else{
            completion_(result.code(), "", 0);
            LOG(ERROR) << "MutateRequest::Done() result="<<result.code();
        }
    }
}


//void ListRequest::SerializeTo(Buffer* request, std::map<int8_t, Buffer>* extras){
//	protocol::ListMessage req_;
//	req_.name_space_	= ns_;
//	req_.key_			= key_;
//	req_.u32limit_		= u32limit_;
//	req_.u8position_	= u8position_;  
//
//    //构造请求，填充到request
//    request->Reset(req_.MaxSize());
//    int len = req_.SerializeTo(request->ptr());
//    request->ReSize(len);
//	LOG(INFO)<<"ListRequest::SerializeTo() create buffer len="<<len;
//}

//void ListRequest::HandleResult(const Buffer &result, 
//							   const std::map<int8_t, Buffer>& extras){
//    if (!response_.ParseFrom(result.ptr(), result.len())) {
//        LOG(ERROR) << " ListRequest::HandleResult（） parse error from buff to response";
//		completion_(ldd::protocol::kNotValidPacket,"", 0);
//        return;
//    }
//
//    LOG(INFO) << "key: " << response_.key_.data()
//			  << ", value: " << response_.value_.data();
//	completion_(response_.s64Version_, response_.value_.ToString(), response_.u64ttl_);
//    return;
//}
//
//void ListRequest::HandleFinalize(){
//	delete rp_;
//    LOG(INFO) << "finalize called";
//}
//
//void ListRequest::HandleTimeout(){
//    LOG(INFO) << "ListRequest::HandleTimeout() timeout called";
//	HandleError();
//}
//
//void ListRequest::HandleError(){
//    LOG(INFO) << "ListRequest::HandleError() error called";
//	if (rp_->ShouldRety(util::Time::CurrentMilliseconds())){
//		client_->List(ns_, key_, u32limit_, u8position_, 
//						completion_, using_host_,  using_port_, rp_);
//	}else{
//		completion_(ldd::protocol::kTimeout, "", 0);
//		delete rp_;
//		LOG(ERROR) << "ListRequest::HandleError() timeout called";
//	}
//}


} // namespace ldd {
} // namespace client {
} // namespace raw {
