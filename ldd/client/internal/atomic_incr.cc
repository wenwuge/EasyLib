#include <map>
#include <glog/logging.h>
#include "ldd/util/time.h"
#include "ldd/protocol/server/store_error.h"
#include "atomic_incr.h"

#include "../redo_simple_policy.h"

namespace ldd { 
namespace client {
namespace raw {

using namespace ldd::net;

AtomicIncrRequest::AtomicIncrRequest(ClientImpl* ct, const std::string& ns,
    const std::string& key,
	int32_t s32operand, int32_t	s32initial,
	uint64_t u64ttl, const CasCompletion& completion, 
	const std::string& using_host, int using_port,
	RedoPolicy* rp) : 
	client_(ct), ns_(ns), 
	key_(key),s32operand_(s32operand),  
	s32initial_(s32initial), u64ttl_(u64ttl),
	cas_completion_(completion), 
	using_host_(using_host),
	using_port_(using_port),
	rp_(rp){
}

bool AtomicIncrRequest::Init(Payload* request,
                    ldd::util::TimeDiff* recv_timeout,
                    ldd::util::TimeDiff* done_timeout){
                        protocol::AtomicIncrMessage aim;
    aim.name_space_	 = ns_;
    aim.key_			 = key_;
    aim.s32operand_	= s32operand_;
    aim.s32initial_	= s32initial_;			// (u32InitialLen)
    aim.u64ttl_		= u64ttl_;

    *recv_timeout = util::TimeDiff::Milliseconds(client_->get_recv_timeout());
    *done_timeout = util::TimeDiff::Milliseconds(client_->get_done_timeout());

    ldd::net::Buffer buf;
    
    //构造请求，填充到request
    buf.Reset(aim.MaxSize());
    int len = aim.SerializeTo(buf.ptr());
    buf.ReSize(len);

    request->SetBody(buf);
    LOG(INFO)<<"AtomicIncrRequest::SerializeTo() create buffer len="<<len;
    return true;
}

bool AtomicIncrRequest::Recv(const Payload& response,
                    ldd::util::TimeDiff* recv_timeout){
    *recv_timeout = util::TimeDiff::Milliseconds(client_->get_recv_timeout());

    if (!response_.ParseFrom(response.body().data(), response.body().size())) {
        cas_completion_(ldd::protocol::kNotValidPacket, "");
        LOG(ERROR) << "parse error from buff to response";
        return false;
    }

    LOG(INFO) << "AtomicIncrRequest::Recv() version=" << response_.s64Version_;
    cas_completion_(response_.s64Version_, response_.value_.ToString());
    return true;
}

void AtomicIncrRequest::Done(const Result& result){
    LOG(INFO)<<"AtomicIncrRequest::Done() result="<<result.code();

    if (result.IsTimedOut()){
    	if (rp_->ShouldRety(util::Time::CurrentMilliseconds())){
    		client_->Incr(ns_,key_, s32operand_, s32initial_, u64ttl_, 
    						cas_completion_, using_host_, using_port_, rp_);
    	}else{
    		cas_completion_(ldd::protocol::kTimeout, "");
    		LOG(ERROR) << "AtomicAppendRequest::HandleError() timeout called";
    		delete rp_;
    	}
    }else if (result.IsFailed()){
        cas_completion_(kFailed, "");
        delete rp_;
        LOG(ERROR) << "MutateRequest::Done() code="<<kFailed;
    }else if (result.IsCanceled()){
        cas_completion_(kCancel, "");
        delete rp_;
        LOG(ERROR) << "MutateRequest::Done() code="<<kCancel;
    }else if (result.IsOk()){
        delete rp_;
        if (result.code() == kOK){
            LOG(INFO)<<"MutateRequest::Done OK!!!";
        }else{
            cas_completion_(result.code(), "");
            LOG(ERROR) << "MutateRequest::Done() result="<<result.code();
        }
    }
    
}




} // namespace ldd {
} // namespace client {
} // namespace raw {


