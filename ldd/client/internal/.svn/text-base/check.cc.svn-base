#include <map>
#include <glog/logging.h>
#include "ldd/util/time.h"
#include "ldd/protocol/server/store_error.h"
#include "check.h"
#include "../redo_simple_policy.h"

namespace ldd { 
namespace client {
namespace raw {

using namespace ldd::net;


CheckRequest::CheckRequest(ClientImpl* ct, const std::string&ns, 
	const std::string& key, 
	const CheckCompletion& complete, 
	const std::string& using_host, 
	int					using_port,
	RedoPolicy* rp) : 
	client_(ct), ns_(ns), key_(key), 
	check_completion_(complete), 
	using_host_(using_host),
	using_port_(using_port),
	rp_(rp){

}

bool CheckRequest::Init(Payload* request,
    ldd::util::TimeDiff* recv_timeout,
    ldd::util::TimeDiff* done_timeout){
    protocol::CheckMessage cm;
    cm.name_space_	 = ns_;
    cm.key_			 = key_;

    *recv_timeout = util::TimeDiff::Milliseconds(client_->get_recv_timeout());
    *done_timeout = util::TimeDiff::Milliseconds(client_->get_done_timeout());

    ldd::net::Buffer buf;

    //构造请求，填充到request
    buf.Reset(cm.MaxSize());
    int len = cm.SerializeTo(buf.ptr());
    buf.ReSize(len);

    request->SetBody(buf);
    LOG(INFO)<<"CheckRequest::SerializeTo() create buffer len="<<len;
    return true;
}

bool CheckRequest::Recv(const Payload& response,
    ldd::util::TimeDiff* recv_timeout){

    *recv_timeout = util::TimeDiff::Milliseconds(client_->get_recv_timeout());

    if (!response_.ParseFrom(response.body().ptr(), response.body().size())) {
        check_completion_(ldd::protocol::kNotValidPacket, 0);
        LOG(ERROR) << "parse error from buff to response";
        return false;
    }

    LOG(INFO) << "CheckRequest::Recv() version=" << response_.s64Version_;
    check_completion_(response_.s64Version_, response_.u64ttl_);
    return true;
}

void CheckRequest::Done(const Result& result){
    LOG(INFO)<<"CheckRequest::Done() result="<<result.code();

    if (result.IsTimedOut()){
        if (rp_->ShouldRety(util::Time::CurrentMilliseconds())){
            client_->Check(ns_, key_, check_completion_, using_host_,  using_port_, rp_);
        }else{
            check_completion_(ldd::protocol::kTimeout, 0);
            LOG(ERROR) << "CheckRequest::HandleError() timeout called";
            delete rp_;
        }
    }else if (result.IsFailed()){
        check_completion_(kFailed, 0);
        delete rp_;
        LOG(ERROR) << "MutateRequest::Done() code="<<kFailed;
    }else if (result.IsCanceled()){
        check_completion_(kCancel, 0);
        delete rp_;
        LOG(ERROR) << "MutateRequest::Done() code="<<kCancel;
    }else if (result.IsOk()){
        delete rp_;
        if (result.code() == kOK){
            LOG(INFO)<<"MutateRequest::Done OK!!!";
        }else{
            check_completion_(result.code(), 0);
            LOG(ERROR) << "MutateRequest::Done() result="<<result.code();
        }
    }
    
}


} // namespace ldd {
} // namespace client {
} // namespace raw {


