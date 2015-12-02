#include <map>
#include <glog/logging.h>
#include "ldd/util/time.h"
#include "ldd/protocol/server/store_error.h"
#include "mutate.h"
#include "../redo_simple_policy.h"


namespace ldd { 
namespace client {
namespace raw {

using namespace ldd::net;


MutateRequest::MutateRequest(ClientImpl* ct, const Mutation& m, 
			  const MutateCompletion& completion,
			  const std::string& using_host,
			  int				using_port,
			  RedoPolicy* rp) : 
	client_(ct), 
	name_space_(m.name_space_.data(), m.name_space_.size()),
	key_(m.key_.data(), m.key_.size()),
	expected_value_(m.expected_value_.data(), m.expected_value_.size()),
	value_(m.value_.data(), m.value_.size()),

	completion_(completion),
	using_host_(using_host), 
	using_port_(using_port),
	rp_(rp){

	request_.name_space_				= name_space_;
	request_.mutation_.key				= key_;
	request_.mutation_.opt_type			= m.opt_type_;
	request_.mutation_.has_expected_version = m.has_expected_version_;
	request_.mutation_.expected_version = m.expected_version_;
	request_.mutation_.has_expected_val = m.has_expected_val_;
	request_.mutation_.expected_value   = expected_value_;
	request_.mutation_.value			= value_;
	request_.mutation_.has_ttl			= m.has_ttl_;
	request_.mutation_.ttl				= m.ttl_;

}


bool MutateRequest::Init(Payload* request,
    ldd::util::TimeDiff* recv_timeout,
    ldd::util::TimeDiff* done_timeout){

    *recv_timeout = util::TimeDiff::Milliseconds(client_->get_recv_timeout());
    *done_timeout = util::TimeDiff::Milliseconds(client_->get_done_timeout());


    ldd::net::Buffer buf;
    buf.Reset(request_.MaxSize());
    int len = request_.SerializeTo(buf.ptr());


    buf.ReSize(len);

    request->SetBody(buf);

    LOG(INFO)<<"MutateRequest::SerializeTo() create buffer len="<<len;   
    return true;
}

bool MutateRequest::Recv(const Payload& response,
    ldd::util::TimeDiff* recv_timeout){

    *recv_timeout = util::TimeDiff::Milliseconds(client_->get_recv_timeout());

    if (!response_.ParseFrom(response.body().ptr(), response.body().len())) {
        LOG(ERROR) << "MutateRequest::HandleResult() parse error from buff to response";
        completion_(ldd::protocol::kNotValidPacket);
        return false; 
    }

    LOG(INFO) << "MutateRequest::Recv() version=" << response_.s64Version_;
    completion_(response_.s64Version_);
    return true;
}

void MutateRequest::Done(const Result& result){
    LOG(INFO)<<"MutateRequest::Done() result="<<result.code();
    if (result.IsTimedOut()){
        if (rp_->ShouldRety(util::Time::CurrentMilliseconds())){
            Mutation mutation(name_space_, key_);
            mutation.name_space_		   = name_space_;
            mutation.key_				   = key_;
            mutation.opt_type_			   = request_.mutation_.opt_type;
            mutation.has_expected_version_ = request_.mutation_.has_expected_version;
            mutation.expected_version_     = request_.mutation_.expected_version;
            mutation.has_expected_val_	   = request_.mutation_.has_expected_val;
            mutation.expected_value_	   = expected_value_;
            mutation.value_				   = value_;
            mutation.has_ttl_			   = request_.mutation_.has_ttl;
            mutation.ttl_				   = request_.mutation_.ttl;
            client_->Mutate(mutation, completion_, using_host_, using_port_, rp_);

        }else{
            completion_(ldd::protocol::kTimeout);
            delete rp_;
            LOG(ERROR) << "MutateRequest::Done() timeout called";
        }
    }else if (result.IsFailed()){
        completion_(kFailed);
        delete rp_;
        LOG(ERROR) << "MutateRequest::Done() code="<<kFailed;
    }else if (result.IsCanceled()){
        completion_(kCancel);
        delete rp_;
        LOG(ERROR) << "MutateRequest::Done() code="<<kCancel;
    }else if (result.IsOk()){
        delete rp_;
        if (result.code() == kOK){
            LOG(INFO)<<"MutateRequest::Done OK!!!";
        }else{
            completion_(result.code());
            LOG(ERROR) << "MutateRequest::Done() result="<<result.code();
        }
    }
}


} // namespace ldd {
} // namespace client {
} // namespace raw {

