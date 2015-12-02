
#include <glog/logging.h>
#include <boost/shared_ptr.hpp>
#include "ldd/protocol/server/store_proto.h"
#include "ldd/protocol/server/store_error.h"
#include "ldd/protocol/coded_stream.h"
#include "ldd/protocol/client/response.h"
#include "ldd/util/time.h"
#include "context.h"
#include "config_manager.h"
#include "task.h"
#include "job.h"



namespace ldd{
namespace storage{


MutateContext::~MutateContext(){
	delete gms_;
	DLOG(INFO)<<"MutateContext::~MutateContext()";
}
MutateContext::MutateContext(ConfigManager* cm, Job* job, 
                             Message* gms, const net::Payload& payload,
                             int recv_timeout, int done_timeout) :
                            cm_(cm),
                            job_(job),
                            gms_(gms),
                            payload_(payload),
                            recv_timeout_(recv_timeout),
                            done_timeout_(done_timeout),
                            s64Version_(0){
                    
    MutateMessageS *syn_msg	= ((MutateMessageS*)gms_);
    DLOG(INFO)<<" MutateContext::MutateContext() payload.ptr="<<(void*)payload_.body().ptr()
        <<" key.ptr="<<(void*)syn_msg->mutation_.key.data()
        <<" key="<<syn_msg->mutation_.key.ToString()<<" syn_version="<<syn_msg->s64syn_version_;
}


bool MutateContext::Init(Payload* request,
                         ldd::util::TimeDiff* recv_timeout,
                         ldd::util::TimeDiff* done_timeout){

    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    *done_timeout = ldd::util::TimeDiff::Milliseconds(done_timeout_);
    const net::Buffer& buffer = payload_.body();
    (const_cast<char*>(buffer.data()))[buffer.size() - 1] = 
                                        ConfigManager::kTypeFromMaster;

    MutateMessageS *syn_msg	= ((MutateMessageS*)gms_);
    char* start = const_cast<char*>(buffer.data());

    *reinterpret_cast<uint64_t*>(&start[buffer.size() - 9])
        = protocol::htonll(syn_msg->s64syn_version_);

    DLOG(INFO)<<" MutateContext::Init() sync version="<<syn_msg->s64syn_version_;

    request->SetBody(payload_.body());
    DLOG(INFO)<<"MutateContext::SerializeTo() make a payload len="<<payload_.body().size();
    return true;
}

bool MutateContext::Recv(const Payload& response,
                         ldd::util::TimeDiff* recv_timeout){
    DLOG(INFO)<<"MutateContext::Recv() get result payload len="
            <<response.body().size();
    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);

    protocol::MutateResponse slave_res;
    slave_res.ParseFrom(response.body().data(), response.body().size());
    s64Version_ = slave_res.s64Version_;
    
    DLOG(INFO)<<"MutateContext::Recv(), Recv , version="<<slave_res.s64Version_;
    return true;
}

void MutateContext::Done(const Result& result){
    if (result.IsOk()){
         DLOG(INFO)<<"MutateContext::Done() result code="<<result.code();
         cm_->SetPrepareTime(util::Time::CurrentMilliseconds());
    }else if(result.IsFailed()){
        s64Version_ = protocol::kStoreFailed;
        LOG(WARNING)<<"MutateContext::Done() result IsFailed";
    }else if(result.IsTimedOut()){
        s64Version_ = protocol::kStoreTimedOut;
        LOG(WARNING)<<"MutateContext::Done() result IsTimedOut";
    }else if (result.IsCanceled()){
        s64Version_ = protocol::kStoreCanceled;
        LOG(WARNING)<<"MutateContext::Done() result IsCanceled";
    }

    MutateMessageS *syn_msg	= ((MutateMessageS*)gms_);
    protocol::MutateResponse	res;
    res.s64Version_	= s64Version_;
    DLOG(INFO)<<"MutateContext::Done, send message to client, version="
        <<res.s64Version_<<" key="<<syn_msg->mutation_.key.ToString();
    job_->AddResult(&res);    // todo this snipper only suit for 1 master, 1 slave
}


AtomicIncrContext::~AtomicIncrContext(){
    delete gms_;
    DLOG(INFO)<<"AtomicIncrContext::~AtomicIncrContext()";
}

bool AtomicIncrContext::Init(Payload* request,
                         ldd::util::TimeDiff* recv_timeout,
                         ldd::util::TimeDiff* done_timeout){
    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    *done_timeout = ldd::util::TimeDiff::Milliseconds(done_timeout_);
    net::Buffer buf;
    int size = gms_->MaxSize();
    buf.Reset(size);
    size_t real_len = gms_->SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    DLOG(INFO)<<"AtomicIncrContext::SerializeTo() make a payload len="<<real_len;
    return true;
}

bool AtomicIncrContext::Recv(const Payload& response,
                             ldd::util::TimeDiff* recv_timeout){
    DLOG(INFO)<<"AtomicIncrContext::Recv() get result payload len="
     <<response.body().size();
    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    return true;
}

void AtomicIncrContext::Done(const Result& result){
    if (result.IsOk()){
        DLOG(INFO)<<"AtomicIncrContext::Done() result code="<<result.code();
    }else{
        LOG(ERROR)<<"AtomicIncrContext::Done() error string="<<result.ToString();
    }
}

AtomicAppendContext::~AtomicAppendContext(){
	delete gms_;
	DLOG(INFO)<<"AtomicAppendContext::~AtomicAppendContext()";
}
bool AtomicAppendContext::Init(Payload* request,
                             ldd::util::TimeDiff* recv_timeout,
                             ldd::util::TimeDiff* done_timeout){
    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    *done_timeout = ldd::util::TimeDiff::Milliseconds(done_timeout_);
    net::Buffer buf;
    int size = gms_->MaxSize();
    buf.Reset(size);
    size_t real_len = gms_->SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    DLOG(INFO)<<"AtomicAppendContext::SerializeTo() make a payload len="<<real_len;
    return true;
}

bool AtomicAppendContext::Recv(const Payload& response,
                               ldd::util::TimeDiff* recv_timeout){
    DLOG(INFO)<<"AtomicIncrContext::Recv() get result payload len="
       <<response.body().size();
    *recv_timeout = ldd::util::TimeDiff::Milliseconds(recv_timeout_);
    return true;
}

void AtomicAppendContext::Done(const Result& result){
    if (result.IsOk()){
        DLOG(INFO)<<"AtomicAppendContext::Done() result code="<<result.code();
    }else{
        LOG(ERROR)<<"AtomicAppendContext::Done() error string="<<result.ToString();
    }
}

boost::shared_ptr<net::OutgoingMsg> ContextFactory::MakeContext(
                                        ConfigManager* cm,
                                        Job* job,
                                        int type, 
                                        protocol::Message* msg, 
                                        const net::Payload& payload, 
                                        int recv_timeout,
                                        int done_timeout){
	boost::shared_ptr<OutgoingMsg> T;
	switch(type){
		case LDD_PROXY_2_STORE_MUTATE:{
				MutateMessageS* syn_msg = new MutateMessageS;
				*syn_msg	= *((MutateMessageS*)msg);  
                DLOG(INFO)<<"ContextFactory::MakeContext() key.size="
                        <<syn_msg->mutation_.key.size()
                        <<" key="<<syn_msg->mutation_.key.ToString();
                T = boost::make_shared<MutateContext>(cm, job, syn_msg, payload,
                                                      recv_timeout,done_timeout);
				return T;
			}
		case LDD_PROXY_2_STORE_ATOMIC_INCR:{
				AtomicIncrMessageS* syn_msg = new AtomicIncrMessageS;
				*syn_msg	= *((AtomicIncrMessageS*)msg);
                T = boost::make_shared<AtomicIncrContext>(cm,job, syn_msg, payload,
                                                        recv_timeout,done_timeout);
				return T;
			}
		case LDD_PROXY_2_STORE_ATOMIC_APPEND:{
				AtomicAppendMessageS* syn_msg = new AtomicAppendMessageS;
				*syn_msg	= *((AtomicAppendMessageS*)msg);
                T = boost::make_shared<AtomicAppendContext>(cm,job, syn_msg, payload,
                                                        recv_timeout,done_timeout);
				return T;
			}
	}
    return T;

}


GetVersionContext:: ~GetVersionContext(){
	DLOG(INFO)<<"GetVersionContext:: ~GetVersionContext()";
}
bool GetVersionContext::Init(Payload* request,
                               ldd::util::TimeDiff* recv_timeout,
                               ldd::util::TimeDiff* done_timeout){
    net::Buffer buf;
    GetVersionMessageS gm;
    *recv_timeout = ldd::util::TimeDiff::Milliseconds(manager_->get_recv_timeout());
    *done_timeout = ldd::util::TimeDiff::Milliseconds(manager_->get_done_timeout());

    gm.host_port_ = host_port_;
    int len = gm.MaxSize();
    buf.Reset(len);
    size_t real_len = gm.SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    DLOG(INFO)<<"GetVersionContext::Init(), payload size="
                <<len<<" real_len="<<real_len
                <<" channel name="<<channel()->name()
                <<" recv_timeout="<<manager_->get_recv_timeout()
                <<" done_timeout="<<manager_->get_done_timeout();
    return true;
}

bool GetVersionContext::Recv(const Payload& response,
                               ldd::util::TimeDiff* recv_timeout){
    *recv_timeout = ldd::util::TimeDiff::Milliseconds(manager_->get_recv_timeout());

    DLOG(INFO)<<"GetVersionContext::Recv(), payload size="<<response.body().size();
    ldd::protocol::GetVersionResponse gvr;
    bool ret = gvr.ParseFrom(response.body().data(), response.body().size());
    if (!ret){
       LOG(ERROR)<<"GetVersionContext::Recv() parse buffer error, len="
                    <<response.body().size()
                    <<" channel name="<<channel()->name();
       std::string r;
       manager_->VersionInfoCallBack(r, 0);
       return true;
    }
    std::string h(gvr.home_host_port_.data(), gvr.home_host_port_.size());
    DLOG(INFO)<<"GetVersionContext::Recv()"<<" home address="
       <<gvr.home_host_port_.ToString()<<" dest_address="
       <<gvr.dest_host_port_.ToString()
       <<" channel name="<<channel()->name();
    manager_->VersionInfoCallBack(h, gvr.s64Version_);
    return true;
}

void GetVersionContext::Done(const Result& result){
    if (result.IsOk()){
        DLOG(INFO)<<"GetVersionContext::Done() result code="<<result.code()
            <<" channel name="<<channel()->name();
    }else{
    	std::string host_port;
    	manager_->VersionInfoCallBack(host_port, 0);
        LOG(ERROR)<<"GetVersionContext::Done() error string="<<result.ToString()
            <<" channel name="<<channel()->name();
    } 
}	

PrepareContext::~PrepareContext(){
	DLOG(INFO)<<"PrepareContext::~PrepareContext() and delete message";
	delete pm_;
};
bool PrepareContext::Init(Payload* request,
                          ldd::util::TimeDiff* recv_timeout,
                          ldd::util::TimeDiff* done_timeout){
   *recv_timeout = ldd::util::TimeDiff::Milliseconds(prepare_result_->get_recv_timeout());
   *done_timeout = ldd::util::TimeDiff::Milliseconds(prepare_result_->get_done_timeout());
    net::Buffer buf;
    int len = pm_->MaxSize();
    buf.Reset(len);
    size_t real_len = pm_->SerializeTo(buf.ptr());
    buf.ReSize(real_len);
    request->SetBody(buf);
    DLOG(INFO)<<"PrepareContext::Init(), payload size="<<real_len;
    return true;
}

bool PrepareContext::Recv(const Payload& response,
                             ldd::util::TimeDiff* recv_timeout){
    *recv_timeout = ldd::util::TimeDiff::Milliseconds(prepare_result_->get_recv_timeout());
    // parse from result
    ldd::protocol::PrepareMessageS pm;
    bool ret = pm.ParseFrom(response.body().data(),response.body().size());
    if (!ret){
     LOG(ERROR)<<"PrepareContext::Recv() parse buffer error, len="
                <<response.body().size();
    }

    DLOG(INFO)<<"PrepareContext::Recv() response payload size="
     <<response.body().size()<<" home_port="<<pm.home_port_.ToString()
     <<" dest_port="<<pm.dest_host_port_.ToString();
    prepare_result_->AddResult(true);	//同样需要check版本号， 因为启动时新的角色还没有分配好，有可能角色不是最新的
    return true;
}

void PrepareContext::Done(const Result& result){
    if (result.IsOk()){
        DLOG(INFO)<<"PrepareContext::Done() result code="<<result.code();
    }else{
        version_ = protocol::kPrepareError;
        prepare_result_->AddResult(false);	
        LOG(ERROR)<<"PrepareContext::Done() error string="<<result.ToString();
    }
}

int PrepareResultH::get_done_timeout(){
    return config_manager_->get_done_timeout();
}
int PrepareResultH::get_recv_timeout(){
    return config_manager_->get_recv_timeout();
}

bool PrepareResultH::SendPrepareMessage(uint64_t& version){
	/*if (!config_manager_->IsMaster()){
		result_num_++;
		LOG(INFO)<<"PrepareResultH::SendPrepareMessage(), I am not master, \
                   add result right now";
		AddResult(true);
		return true;
        }*/

    if (!config_manager_->NeedDoPrepare()){
        result_num_++;
        DLOG(INFO)<<"PrepareResultH::SendPrepareMessage(), no need send prepare this opt";
        AddResult(true);
        return true;
    }

	// only master check this state
	if (!config_manager_->IsReady()){
		LOG(WARNING)<<"PrepareResultH::SendPrepareMessage() \
                      warning roles ara not ready, return ";
        result_num_++;
        AddResult(false);
        version = protocol::kRolesNotReady;
		return false;
	}
	
	const std::vector<boost::shared_ptr<Channel> >* vec_channel = NULL;
	vec_channel = config_manager_->GetSlaveChannels(); // todo handle the error return 
	if (!vec_channel || vec_channel->size() == 0){
		LOG(ERROR)<<"PrepareResultH::SendPrepareMessage(), \
                    GetSlaveChannels() error";
        result_num_++;
        /*AddResult(false);
        version = protocol::kSlaveNotReady;*/
        AddResult(true);
		return false;
	}	
	
	std::string& home_port = config_manager_->GetHomeHostPort();
	Slice		 host_port_ = home_port;
	DLOG(INFO)<<"PrepareResultH::SendPrepareMessage() home_port="
			 <<home_port<<" slave_size="<<vec_channel->size();
	
	for (size_t i=0; i<vec_channel->size(); i++){
		protocol::PrepareMessageS* pm = new ldd::protocol::PrepareMessageS(); 
		pm->home_port_	= host_port_;
        boost::shared_ptr<PrepareContext> context(new PrepareContext(this, pm, version));
        (*vec_channel)[i]->Post(context, 
                            util::TimeDiff::Milliseconds(500), true);		// todo how to handle when fail
		result_num_++;
	}
	
	DLOG(INFO)<<"PrepareResultH::SendPrepareMessage(), \
               need reponse result size="<<result_num_;
	return true;
}

bool PrepareResultH::AddResult(bool right){
    

	enable_write = right;
	result_num_--;
	task_->prepare_checked();
	DLOG(INFO)<<"PrepareResultH::AddResult(), remain result num="<<result_num_;
	if (result_num_ == 0){
        if (right){
            config_manager_->SetPrepareTime(util::Time::CurrentMilliseconds());
        }
		DLOG(INFO)<<"PrepareResultH::AddResult(), enable_write()="<<enable_write
            <<" DoExecute()";
		task_->enable_mutate(enable_write);
		//task_->ReSchedule();
        task_->DoExecute();
	}
    
	return true;
}


bool SynWriteMessageHandle::SynToSlaves(Job* job, int type, 
                                        protocol::Message* msg, 
                                        const net::Payload& payload){
	if (config_manager_ == NULL){
		LOG(ERROR)<<"SynWriteMessageHandle::SynToSlaves() config_manager is null";
		return false;
	}
	if (!config_manager_->IsMaster()){
		DLOG(INFO)<<"SynWriteMessageHandle::SynToSlaves() I am not master do nothing";
		return true;
	}

	const std::vector<boost::shared_ptr<Channel> >* vec_channel;
	vec_channel	= config_manager_->GetSlaveChannels(); // todo handle the error return 
	if (!vec_channel){
		LOG(ERROR)<<"SynWriteMessageHandle::SynToSlaves(), GetSlaveChannels() error";
		return false;
	}	

	std::string& home_port = config_manager_->GetHomeHostPort();
	Slice		 host_port_ = home_port;
	DLOG(INFO)<<"SynWriteMessageHandle::SynToSlaves()     SynToSlaves home_port="
			 <<home_port<<" slave_size="<<vec_channel->size();

	for (size_t i=0; i<vec_channel->size(); i++){
        boost::shared_ptr<OutgoingMsg> context = 
                    ContextFactory::MakeContext(config_manager_,job, type, 
                    msg, payload, 
                    config_manager_->get_recv_timeout(), 
                    config_manager_->get_done_timeout());
		if (!context){
			LOG(ERROR)<<"SynWriteMessageHandle:: create context, type="<<type;
			return false;
		}
		(*vec_channel)[i]->Post(context, util::TimeDiff::Milliseconds(500), true);		// todo how to handle when fail
	}

	if (vec_channel->size() == 0){
		LOG(ERROR)<<"SynWriteMessageHandle:: slaves channel has not created";
		return false;
	}
	return true;
};







}//namespace storage
}//namespace ldd


