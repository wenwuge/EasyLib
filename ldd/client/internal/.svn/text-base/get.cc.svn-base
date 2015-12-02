#include <map>
#include <glog/logging.h>
#include "ldd/util/time.h"
#include "ldd/protocol/server/store_error.h"
#include "get.h"
#include "../redo_simple_policy.h"

namespace ldd { 
namespace client {
namespace raw {

using namespace ldd::net;

GetRequest::GetRequest(ClientImpl* ct, const std::string&ns, const std::string& key, 
	const DataCompletion& complete, 
	const std::string& using_host, 
	int					using_port,
	RedoPolicy* rp) : 
	client_(ct), ns_(ns), key_(key), 
	get_completion_(complete), 
	using_host_(using_host),
	using_port_(using_port),
	rp_(rp){

}


bool GetRequest::Init(Payload* request,
    ldd::util::TimeDiff* recv_timeout,
    ldd::util::TimeDiff* done_timeout){
    protocol::GetMessage gm;
    gm.name_space_	 = ns_;
    gm.key_			 = key_;

    *recv_timeout = util::TimeDiff::Milliseconds(client_->get_recv_timeout());
    *done_timeout = util::TimeDiff::Milliseconds(client_->get_done_timeout());

    ldd::net::Buffer buf;

    buf.Reset(gm.MaxSize());
    int len = gm.SerializeTo(buf.ptr());
    buf.ReSize(len);

    request->SetBody(buf);

    LOG(INFO)<<"GetRequest::SerializeTo() create buffer len="<<len;
    return true;
}

bool GetRequest::Recv(const Payload& response,
    ldd::util::TimeDiff* recv_timeout){

    *recv_timeout = util::TimeDiff::Milliseconds(client_->get_recv_timeout());

    if (!response_.ParseFrom(response.body().ptr(), response.body().size())) {
        LOG(ERROR) <<"GetRequest::HandleTimeout() parse error from buff to response";
        get_completion_(ldd::protocol::kNotValidPacket, "", 0);
        return false;
    }

    DLOG(INFO) << " GetRequest::Recv() key=" << response_.key_.ToString()
        << ", value.size=" << response_.value_.size();
    get_completion_(response_.s64Version_, response_.value_.ToString(), response_.u64ttl_);

    return true;
}

void GetRequest::Done(const Result& result){
    LOG(INFO)<<"GetRequest::Done() result="<<result.code();

    if (result.IsTimedOut()){
    	if (rp_->ShouldRety(util::Time::CurrentMilliseconds())){
    		client_->Get(ns_, key_, get_completion_, using_host_,  using_port_, rp_);
    		LOG(INFO)<<"GetRequest::HandleError() retry ";
    	}else{
    		get_completion_(ldd::protocol::kTimeout, "", 0);
    		LOG(ERROR) << "GetRequest::HandleError() timeout called";
    		delete rp_;
    	}
    }else if (result.IsFailed()){
        get_completion_(kFailed, "", 0);
        delete rp_;
        LOG(ERROR) << "GetRequest::Done() code="<<kFailed;
    }else if (result.IsCanceled()){
        get_completion_(kCancel, "", 0);
        delete rp_;
        LOG(ERROR) << "GetRequest::Done() code="<<kCancel;
    }else if (result.IsOk()){
        delete rp_;
        if (result.code() == kOK){
            LOG(INFO)<<"GetRequest::Done OK!!!";
        }else{
            get_completion_(result.code(), "", 0);
            LOG(ERROR) << "GetRequest::Done() result="<<result.code();
        }
    }
    
}



} // namespace ldd {
} // namespace client {
} // namespace raw {


