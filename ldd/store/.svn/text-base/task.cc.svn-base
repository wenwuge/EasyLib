#include <glog/logging.h>
#include <arpa/inet.h>
#include "ldd/protocol/server/store_error.h"
#include "ldd/protocol/server/store_proto.h"
#include "ldd/protocol/client/response.h"
#include "ldd/protocol/coded_stream.h"
#include "task.h"
#include "job.h"
#include "context.h"
#include "db/db.h"
#include "string_util.h"
#include "config_manager.h"


namespace ldd{
namespace storage{

static protocol::StoreError GetErrorCode(Status& s){
	if (s.ok()){
		return  protocol::kOk;
	}else if (s.IsNotFound()){
		return protocol::kNotFound;
	}else if (s.IsCorruption()){
		return protocol::kCorruption;
	}else if (s.IsIOError()){
		return protocol::kIOError;
	}else {
		return protocol::kUnrecoveralError; 
	}
}
	
Task::~Task(){
	DLOG(INFO)<<"Task::~Task()";
}

void Task::SetNextTask(Task* task) {
	next_	= task;
}

void Task::SetPriorTask(Task* task){
	prior_	=  task;
}

void Task::Run(){
	DLOG(INFO)<<"Task::Run()";
	Execute();		// 异步重新调用后是否有重复运行
}

void Task::ReSchedule(){			
	DLOG(INFO)<<"Task::ReSchedule(), task_id="<<Seq();
	//job_->AddTask(this, false);
    job_->RescheduleTask(this, false);
}

TaskType Task::GetTypeTask(){
	return type_;
}

GetTask::~GetTask(){
	delete msg_;
	delete reponse_;
}

void GetTask::Execute(){
	uint64_t ttl = 0;
	uint64_t version = 0;
	Status s =db_->Get(msg_->u16BucketID_, msg_->u16Ns_, 
						        msg_->key_, Slice(), &value_, &version, &ttl);
	if (!s.ok()){
		version = GetErrorCode(s); // protocol::kGetDbError;	// todo define a few error type
		LOG(WARNING)<<"GetTask::Execute() db->get error, info:"<<s.ToString()
			    <<" ns="<<msg_->u16Ns_<<" bucketid="<<msg_->u16BucketID_
			    <<" key="<<msg_->key_.ToString()<<" key_len="<<msg_->key_.size();
	}else{
		DLOG(INFO)<<"GetTask::Execute() OK!!! "<<" bucketid="
			<<msg_->u16BucketID_<<" ns="<<msg_->u16Ns_<<" key="<<msg_->key_.ToString()
            <<" len="<<msg_->key_.size()<<" value.size="<<value_.size();
	}
	
	// must new a reponse_, will be used by modify task
	reponse_ = new protocol::GetOrListResponse();
	protocol::GetOrListResponse* res = 
                            static_cast<protocol::GetOrListResponse*>(reponse_);
	res->s64Version_ 	= version;  	
	res->key_   		= msg_->key_;
	res->value_ 		= value_;			// need copy 
	res->u64ttl_		= ttl;
	DLOG(INFO)<<"GetTask::Execute(), get result "<<"namespace="<<msg_->u16Ns_
				<<" s64Version_="<<res->s64Version_<<" value:"<<value_.size();
	
	if(next_){		
		DLOG(INFO)<<"Task::Run() add next task to queue";
		job_->AddTask(next_, true);
	}

	if (type_ == kTypeRead){  // may be kTypeSubRead, no need add to job
		job_->AddResult(res);		
	}
}

CheckTask::~CheckTask(){
	delete msg_;
}

void CheckTask::Execute(){
	uint64_t ttl = 0;
	uint64_t version = 0;
	Status s = db_->Get(msg_->u16BucketID_, msg_->u16Ns_, 
						msg_->key_, Slice(), NULL, &version, &ttl);
	if (!s.ok()){
		version = GetErrorCode(s); // protocol::kCheckDbError;	// todo define a few error type
		LOG(WARNING)<<"CheckTask::Execute() db->get error, info:"<<s.ToString();
	}else{
		DLOG(INFO)<<"CheckTask::Execute() OK!!! "<<" bucketid="
				<<msg_->u16BucketID_<<" key="<<msg_->key_.ToString();
	}

	protocol::CheckResponse reponse;
	reponse.s64Version_		= version;
	reponse.key_			= msg_->key_;
	reponse.u64ttl_			= ttl;
	
	DLOG(INFO)<<"CheckTask::Execute(), get result "<<"namespace="<<msg_->u16Ns_
				<<" bucketid="<<msg_->u16BucketID_<<" version="
				<<reponse.s64Version_<<" ttl"<<ttl;

	if(next_){		
		DLOG(INFO)<<"Task::Run() add next task to queue";
		job_->AddTask(next_, true);
	}

	job_->AddResult(&reponse);
}


ModifyBaseTask::~ModifyBaseTask(){
	DLOG(INFO)<<"ModifyBaseTask::~ModifyBaseTask() and delete  PrepareResultH ";
	delete prepare_handle_;
	delete syn_handle_;
}

void ModifyBaseTask::Execute(){
    DLOG(INFO)<<"ModifyBaseTask::Execute() task_id="<<Seq();
	//if (!prepared()){
        bool ret = config_manager_->IsFromMaster(GetMsgFrom());
        if (!ret){
            version_ = protocol::kMsgNotFromMaster;
            enable_mutate(false);
            LOG(ERROR)<<"ModifyBaseTask::Execute() i am slave:msg not from master, \
                        return null, disable_write";
            DoExecute();
            return;
        }

        if (!config_manager_->IsMaster()){    // todo this condition check should be move ahead
            version_ = GetSynVersion();
            DLOG(INFO)<<"ModifyBaseTask::Execute(), GetSynVersion()="<<version_;
            if (version_ < 0){
                LOG(ERROR)<<"error ModifyBaseTask::Execute(), GetSynVersion()="<<version_;
                enable_mutate(false);
                version_ = protocol::kSynVersionError;
            }else{
                enable_mutate(true);
                DLOG(INFO)<<"ModifyBaseTask::Execute() enable_mutate, DoExecute()!!!";
            }
            DoExecute();
            return;
        }else {
            //if (!config_manager_->IsMaster()){
	        prepare_handle_ = new PrepareResultH(config_manager_, this);
	        DLOG(INFO)<<"ModifyBaseTask::Execute(), SendPrepareMessage";
	        bool ret = prepare_handle_->SendPrepareMessage(version_);
	        if (!ret){
		        LOG(ERROR)<<"ModifyBaseTask::Execute(), SendPrepareMessage error Finalize()";
	        }
            return;	// must need "return",  otherwise will fail when this node is a slave
        }
	//}
	/*if (prepared()){
		bool ret = config_manager_->IsFromMaster(GetMsgFrom());
		if (!ret){
			version_ = protocol::kMsgNotFromMaster;
			enable_mutate(false);
            LOG(ERROR)<<"ModifyBaseTask::Execute() i am slave:msg not from master, \
                        return null, disable_write";
		}
		LOG(INFO)<<" ModifyBaseTask::Execute() start --------- DoExecute() task_id="<<Seq();
		DoExecute();
	}*/
}

MutateTask::~MutateTask(){
    delete msg_;
}
int64_t MutateTask::GetSynVersion() {
    return msg_->s64syn_version_;
}
uint8_t MutateTask::GetMsgFrom(){
    return msg_->u8from_;
}
void MutateTask::DoExecute(){
	Slice    insert_value;
    DLOG(INFO)<<"MutateTask::DoExecute(), task id="<<Seq();
	if (can_mutate()){
		// check the expected_version or expected_value
		if (msg_->mutation_.has_expected_val || msg_->mutation_.has_expected_version){	
			if (prior_){
				Response* resp = (static_cast<GetTask*>(prior_))->GetResponse();
				protocol::GetOrListResponse* res = 
                    static_cast<protocol::GetOrListResponse*>(resp);
				if (res->s64Version_ < 0){
					version_ = res->s64Version_;
					LOG(ERROR)<<"MutateTask::DoExecute() first get error, version="<<res->s64Version_;
					goto ret;
				}
				
				Slice value = res->value_;
				if ( (msg_->mutation_.has_expected_val) && 
                    (value.Compare(msg_->mutation_.expected_value) != 0)){
					version_ = protocol::kNotMatchValue;
					LOG(ERROR)<<"MutateTask::DoExecute() \
                                value check error expected_val="
						<<msg_->mutation_.expected_value.ToString()
                        <<" value="<<value.ToString();
					goto ret;
				}else{DLOG(INFO)<<"MutateTask::DoExecute() val check OK!";}
				if (msg_->mutation_.has_expected_version && 
                    (res->s64Version_ != msg_->mutation_.expected_version )){
					version_ = protocol::kNotMatchVersion;
					LOG(ERROR)<<"MutateTask::DoExecute() version check error!\
                                expected_version="
						<<msg_->mutation_.expected_version
                        <<" version_="<<res->s64Version_;
					goto ret;
				}else{DLOG(INFO)<<"MutateTask::DoExecute() version check OK!"
						<<msg_->mutation_.expected_version<<" version_="
                        <<res->s64Version_;}
			}else{
				version_ = protocol::kInvalidPriorTask;
				LOG(ERROR)<<"MutateTask::DoExecute() first No get task";
				goto ret;
			}
		} // end check the expected_version or expected_value
		
		if (msg_->mutation_.opt_type == 1){
			Status s = db_->Put(msg_->u16BucketID_, msg_->u16Ns_, msg_->mutation_.key, 
								Slice(),msg_->mutation_.value, 
                                &version_, msg_->mutation_.ttl);
            //Status s;
            DLOG(INFO) << "MutateTask::DoExecute() Writing bucketid="
                << msg_->u16BucketID_<<" ns="<<msg_->u16Ns_
                <<" row.size="<<msg_->mutation_.key.size()
                <<" row=" << msg_->mutation_.key.ToString()
                <<" ver=" << version_<<" ttl="<<msg_->mutation_.ttl
                <<" value.size="<<msg_->mutation_.value.size();
			if (!s.ok()){
				version_ =  GetErrorCode(s); // todo define a few error type
				LOG(WARNING)<<"MutateTask::Execute() db->Put error, info:"
                        <<s.ToString()<<" version="<<(int64_t)version_;
                goto ret;
			}else{
				msg_->s64syn_version_ = version_;
				DLOG(INFO)<<"MutateTask::Execute() db->Put OK!!! bucketid="
                    <<msg_->u16BucketID_<<" ns="<<msg_->u16Ns_<<" ttl="
                    <<msg_->mutation_.ttl<<" version="<<version_;
			}
		}else if (msg_->mutation_.opt_type == 2){
			Status s = db_->Del(msg_->u16BucketID_, msg_->u16Ns_,
                                    msg_->mutation_.key, Slice(), &version_);
			if (!s.ok()){
				version_ =  GetErrorCode(s); //	// todo define a few error type
				LOG(WARNING)<<"MutateTask::Execute() db->del error, info:"<<s.ToString();
				goto ret;
			}else{
				DLOG(INFO)<<"MutateTask::Execute() db->del OK!!!"
                        <<" bucketid="<<msg_->u16BucketID_
						<<" ns="<<msg_->u16Ns_<<" key="
                        <<msg_->mutation_.key.ToString()<<" version="<<version_;
			}
		}else{
			LOG(ERROR)<<"mutation error OptType="<<(uint32_t)msg_->mutation_.opt_type;
			goto ret;
		}
		DLOG(INFO)<<"MutateTask::Execute(), write on this compute, version="<<version_;

        if (config_manager_->IsMaster()){
            syn_handle_ = new SynWriteMessageHandle(config_manager_);
            bool ret2 = syn_handle_->SynToSlaves(job_, LDD_PROXY_2_STORE_MUTATE, msg_, payload_);
            if (!ret2){
                LOG(ERROR)<<"MutateTask::Execute(), SynToSlaves error";
            }
        }//if (config_manager_->IsMaster()){
	}//if (can_mutate()){
	
ret:
	if(next_){		
		DLOG(INFO)<<"Task::Run() add next task to queue";    // this will never be executed
		job_->AddTask(next_, true);
	}

    // version < 0  || i am slave
    if ((int64_t)version_ < 0 || !config_manager_->IsMaster()){
        protocol::MutateResponse	res;
        res.s64Version_	= version_;
        DLOG(INFO)<<"MutateTask::Execute(), add result to client, version="
            <<res.s64Version_<<" IsMaster="<<config_manager_->IsMaster();
        job_->AddResult(&res);
    }
}

ListTask::~ListTask(){
	delete msg_;
}
void ListTask::Execute(){
	Iterator* itr = NULL;
	if (msg_->u8position_ == 1){
		itr = db_->ListFrom(msg_->u16Ns_, msg_->key_, Slice(), msg_->u32limit_);
		DLOG(INFO)<<"ListTask::Execute(), ListFrom()"<<" limit="<<msg_->u32limit_
			<<" ns="<<msg_->u16Ns_<<" start_key="<<msg_->key_.ToString();
	}else if (msg_->u8position_ == 2){
		itr = db_->ListAfter(msg_->u16Ns_, msg_->key_, Slice(), msg_->u32limit_);
		DLOG(INFO)<<"ListTask::Execute(), ListAfter(), namespace="<<msg_->u16Ns_
			<<" limit="<<msg_->u32limit_<<" ns="<<msg_->u16Ns_
            <<" key="<<msg_->key_.ToString();
	}else{
		LOG(ERROR)<<"ListTask::Execute() error position="
            <<(uint32_t)msg_->u8position_;
		goto ret;
	}
	while (itr->Valid()){
		protocol::GetOrListResponse response;
		response.s64Version_		= itr->seq();
		response.key_				= itr->rowkey();
		response.value_				= itr->value();
		response.u64ttl_			= itr->ttl();
		DLOG(INFO)<<"ListTask::Execute(), iterator namespace="<<msg_->u16Ns_
			<<" limit="<<msg_->u32limit_<<" key="<<response.key_.ToString()
			<<" value="<<response.value_.ToString();
        job_->AddResult(&response);
		itr->Next();
	}
	delete itr;

ret:

	if(next_){		
		DLOG(INFO)<<"Task::Run() add next task to queue";
		job_->AddTask(next_, true);
	}

	DLOG(INFO)<<"ListTask::Execute() Finalize()";
	job_->Done();
}

AtomicIncrTask::~AtomicIncrTask(){
	delete msg_;
}
int64_t AtomicIncrTask::GetSynVersion() {
	return msg_->s64syn_version_;
}
uint8_t AtomicIncrTask::GetMsgFrom(){
	return msg_->u8from_;
}
void AtomicIncrTask::DoExecute(){ 
	Slice ret_value;
	int32_t put_value = 0;
	if (can_mutate()){
		if (prior_ != NULL){
			Response* resp = (static_cast<GetTask*>(prior_))->GetResponse();
			protocol::GetOrListResponse* res = 
                static_cast<protocol::GetOrListResponse*>(resp);
			if (res->s64Version_ < protocol::kOk && 
                res->s64Version_ != protocol::kNotFound){
				version_ = res->s64Version_;
				LOG(ERROR)<<"AtomicIncrTask::DoExecute() \
                            first get fatal error, version="<<res->s64Version_;
				goto ret;
			}
			
			if (res->s64Version_ > protocol::kOk){
				if (res->value_.size() != sizeof(int32_t) ){
					version_  = protocol::kInvalidOperation;
					LOG(ERROR)<<"AtomicIncrTask::DoExecute() \
                                InvalidOperation, value size not valid value="
							  << res->value_.ToString();
					goto ret;
				}
				uint32_t v = 	*(uint32_t*)(res->value_.data());
				put_value = ntohl(v);
				put_value += msg_->s32operand_;
				DLOG(INFO)<<"AtomicIncrTask::DoExecute() get ok"
                    <<" value="<<res->value_.ToString()
					<<" value="<<put_value<<" operand="<<msg_->s32operand_;
				put_value = htonl((uint32_t)put_value);
			}else if (res->s64Version_ == protocol::kNotFound){
				put_value = htonl((uint32_t)msg_->s32initial_);
				DLOG(INFO)<<"AtomicIncrTask::DoExecute() NotFound"
                    <<" value"<<" default="<<msg_->s32initial_;
			}
			char* ptr = (char*)(&put_value);
			Slice insert_value(ptr, sizeof(int32_t));
			
			Status s = db_->Put(msg_->u16BucketID_, msg_->u16Ns_, 
                                msg_->key_, Slice(), insert_value, 
                                &version_, msg_->u64ttl_);
			LOG(INFO)<<"AtomicIncrTask::Execute(), put";
			if (!s.ok()){
				version_ =  GetErrorCode(s); //protocol::kIncreDbError;	// todo define a few error type
				LOG(WARNING)<<"CheckTask::Execute() db->get error, info:"
                    <<s.ToString();
				goto ret;
			}else{
				msg_->s64syn_version_ = version_;
				ret_value = insert_value;
				DLOG(INFO)<<"AtomicIncrTask::Execute(), incr OK!!! version="<<version_
						 <<" ttl="<<msg_->u64ttl_<<" value="<<insert_value.ToString();
			}

			syn_handle_ = new SynWriteMessageHandle(config_manager_);
			bool ret2 = syn_handle_->SynToSlaves(job_, LDD_PROXY_2_STORE_ATOMIC_INCR,
                                                 msg_, payload_);
			if (!ret2){
				LOG(ERROR)<<"MutateTask::Execute(), SynToSlaves error";
			}
		}//if (prior_ != NULL){
	} // if (can_mutate()){

ret:

	if(next_){		
		DLOG(INFO)<<"Task::Run() add next task to queue";
		job_->AddTask(next_, true);
	}

	protocol::CasResponse res;
	res.s64Version_		= version_;
	res.key_			= msg_->key_;
	res.value_			= ret_value;			
	DLOG(INFO)<<"AtomicIncrTask::Execute(), respond to client, version="
                <<res.s64Version_
			<<" key="<<res.key_.ToString()<<" value="<<ret_value.ToString();
	job_->AddResult(&res);
}

AtomicAppendTask::~AtomicAppendTask(){
	delete msg_;
}
int64_t AtomicAppendTask::GetSynVersion() {
	return msg_->s64syn_version_;
};
uint8_t AtomicAppendTask::GetMsgFrom(){
	return msg_->u8from_;
}
void AtomicAppendTask::DoExecute(){
	Slice    insert_value = new_value_;
	if (can_mutate()){
		if (prior_ != NULL){
			Response* resp = (static_cast<GetTask*>(prior_))->GetResponse();
			protocol::GetOrListResponse* res = 
                static_cast<protocol::GetOrListResponse*>(resp);
			if (res->s64Version_ >= 0){
				LOG(INFO)<<"AtomicAppendTask::DoExecute()  \
                    OK get right, get version="<<res->s64Version_;
			}else{
				LOG(ERROR)<<"AtomicAppendTask::DoExecute()  \
                            error get version="<<res->s64Version_;
				version_ = res->s64Version_;
				goto ret;
			}
			
			if (msg_->u8position_ == 1){
				new_value_.assign(msg_->content_.data(), msg_->content_.size());
				new_value_.append(res->value_.data(),res->value_.size());
			}else if (msg_->u8position_ == 2){
				new_value_.assign(res->value_.data(),res->value_.size());
				new_value_.append(msg_->content_.data(), msg_->content_.size());
			}else{
				version_ = protocol::kInvalidParameter;
				LOG(ERROR)<<" invalid positon="<<(uint32_t)msg_->u8position_;
				goto ret;
			}
			insert_value = new_value_;
			DLOG(INFO)<<"AtomicAppendTask::DoExecute() ttl="
                <<msg_->u64ttl_<<" new value="<<new_value_;
			Status s = db_->Put(msg_->u16BucketID_, 
                                msg_->u16Ns_, msg_->key_, Slice(), 
                                insert_value, &version_, msg_->u64ttl_);
			if (!s.ok()){
				version_ =  GetErrorCode(s); //protocol::kAppendDbError;	// todo define a few error type
				LOG(WARNING)<<"AtomicAppendTask::Execute() \
                              put error, info:"<<s.ToString();
				goto ret;
			}else{ 
				msg_->s64syn_version_ = version_;
				LOG(INFO)<<"AtomicAppendTask::Execute() put ok seq="<<version_; 
			}

			LOG(INFO)<<"AtomicAppendTask::Execute(), write on this compute";
			syn_handle_ = new SynWriteMessageHandle(config_manager_);
			bool ret2 = syn_handle_->SynToSlaves(job_, LDD_PROXY_2_STORE_ATOMIC_APPEND,
                                                 msg_, payload_);
			if (!ret2){
				LOG(ERROR)<<"MutateTask::Execute(), SynToSlaves error";
			}
		} //if (prior_ != NULL){
	}	// if (can_mutate()){

ret:

	if(next_){		
		DLOG(INFO)<<"Task::Run() add next task to queue";
		job_->AddTask(next_, true);
	}

	protocol::CasResponse res;
	res.s64Version_		= version_;
	res.key_			= msg_->key_;
	res.value_			= insert_value;			// todo whether need delete resource
	DLOG(INFO)<<"AtomicAppendTask::Execute(), respond to client, seq="
              <<res.s64Version_
			 <<" key="<<res.key_.ToString()<<" value="<<insert_value.ToString();
	job_->AddResult(&res);
}
	
GetVersionTask::~GetVersionTask(){
	delete msg_;
}
void GetVersionTask::Execute(){
	protocol::GetVersionResponse res;
	res.home_host_port_	= home_port_;
	res.dest_host_port_	= msg_->host_port_;
	res.s64Version_		= db_->LastSequence();				// reserved field
	DLOG(INFO)<<"GetVersionTask::Execute() set value home_address="
             <<home_port_.ToString()
			 <<" dest_address="<<msg_->host_port_.ToString()
             <<" version="<<res.s64Version_;
	job_->AddResult(&res);
}

}//namespace storage
}//namespace ldd


