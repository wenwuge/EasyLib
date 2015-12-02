#include <glog/logging.h>
#include "session.h"
#include "task.h"
#include "job.h"
#include "config_manager.h"



namespace ldd{
namespace storage{

Session::~Session(){
	DLOG(INFO)<<"Session::~Session()";
	for (size_t i=0; i<task_vec_.size(); i++){
		DLOG(INFO)<<"Session::~Session(), delete task seq="<<task_vec_[i]->Seq();
        delete task_vec_[i];
	}		
	//delete job_;
}

void Session::SetBody(net::Buffer buf){
   bool ret = response_.SetBody(buf);
   if (!ret){
       LOG(INFO)<<"Session<N>::SetBody(), set body error";
   }
   DLOG(INFO)<<"Session::SetBody() send to client, fowllowing emit buffer size="
            <<buf.size();
   Notify();
}

void Session::Done(){
    DLOG(INFO)<<"Session::Done() only call by list interface notify server";
    Notify(true);
}

void ReadSession::Init(const Payload& request, Action* next){
	buffer_ = request;
	GetMessageS* oGetMessages = new GetMessageS;
	bool ret = oGetMessages->ParseFrom(request.body().data(), 
                                        request.body().size());
	if (!ret){
		LOG(ERROR)<<"ReadSession::Init() error len=%d"
                            <<request.body().size();
        channel()->Close();
        delete oGetMessages;
		return ;
	}
	
    job_ = new Job(boost::static_pointer_cast<ReadSession>(shared_from_this()));
	DLOG(INFO)<<"ReadSession::Init() get a payload, key="
                <<oGetMessages->key_.ToString()
				<<" body.size()="<<request.body().size()<<" u16Ns="<<oGetMessages->u16Ns_;
	GetTask* task	  = new GetTask(oGetMessages, kTypeRead, 
                                job_, config_manager_->GetDb());
	DLOG(INFO)<<"ReadSession::Init() add a new task, ns="<<oGetMessages->u16Ns_
				<<" BucketID="<<oGetMessages->u16BucketID_;
	task_vec_.push_back(task);
	job_->AddTask(task, true);
    DLOG(INFO)<<"ReadSession::Init() GetTask id="<<task->Seq();
    *next = kWait;
}

void ReadSession::Emit(Payload* response, Action* next){
    *next = kDone;
    *response = response_;
    DLOG(INFO)<<"ReadSession::Emit() response size="<<response->body().size();
}

void ReadSession::Done(Code* code){
    *code = 0;
    DLOG(INFO)<<"ReadSession::Done()";
    delete job_;
}

void ReadSession::Cancel() {
    DLOG(INFO)<<"ReadSession::Cancel()";
}

void CheckSession::Init(const Payload& request, Action* next){
	buffer_ = request;
	CheckMessageS* msg = new CheckMessageS;
	bool ret = msg->ParseFrom(request.body().data(), request.body().size());
	if (!ret){
		LOG(ERROR)<<"CheckSession::Init() error len=%d"<<request.body().size();
        channel()->Close();
        delete msg;
		return ;
	}
	DLOG(INFO)<<"CheckSession::Init() get a payload, size= "
                <<msg->key_.ToString()
				<<request.body().size()<<" u16Ns_ "<<msg->u16Ns_;
                
	job_ = new Job(boost::static_pointer_cast<CheckSession>(shared_from_this()));

	CheckTask* task = new CheckTask(msg, kTypeRead, job_, 
                                    config_manager_->GetDb());
	task_vec_.push_back(task);
	job_->AddTask(task, true);
    DLOG(INFO)<<"CheckSession::Init() CheckTask id="<<task->Seq();
    *next = kWait;
}

void CheckSession::Emit(Payload* response, Action* next){
    *next = kDone;
    *response = response_;
    DLOG(INFO)<<"CheckSession::Emit() response size="<<response->body().size();
}

void CheckSession::Done(Code* code){
    *code = 0;
    DLOG(INFO)<<"CheckSession::Done()";
    delete job_;
}

void CheckSession::Cancel() {
    DLOG(INFO)<<"CheckSession::Cancel()";
}

void MutateSession::Init(const Payload& request, Action* next){
    buffer_ = request;
	MutateMessageS* msg = new MutateMessageS;
	bool ret = msg->ParseFrom(request.body().data(), request.body().size());
	if (!ret){
		LOG(ERROR)<<"MutateSession::Init() error len=%d"
            <<request.body().size();
        channel()->Close();
        delete msg;
        return ;
	}	
	DLOG(INFO)<<"MutateSession::Init() get a payload, size= "
				<<request.body().size()<<" u16Ns_="<<msg->u16Ns_
				<<" bucketid="<<msg->u16BucketID_
                <<" Payload.ptr="<<((void*)request.body().ptr())
                <<" key.ptr="<<(void*)msg->mutation_.key.data()
                <<" key="<<msg->mutation_.key.ToString()
                <<" channel name="<<channel()->name();
	job_ = new Job(boost::static_pointer_cast<MutateSession>(shared_from_this()));
	if (config_manager_->IsMaster()){
		msg->u8from_ = config_manager_->GetFromTypeofMaster();
	}
	MutateTask* task	= new MutateTask(config_manager_, msg,request, 
                                        kTypeWrite, 
										job_, config_manager_->GetDb());
	task_vec_.push_back(task);

	GetTask* get_task = NULL;
	if (msg->mutation_.has_expected_val || msg->mutation_.has_expected_version){
		// create a read task
		GetMessageS * get_msg = new GetMessageS;
		get_msg->u16Ns_		   = msg->u16Ns_;
		get_msg->key_		   = msg->mutation_.key;
		get_msg->u16BucketID_  = msg->u16BucketID_;
		get_task			  = new GetTask(get_msg, kTypeReadSubTask, 
											job_, config_manager_->GetDb());
		task_vec_.push_back(get_task);
		get_task->SetNextTask(task);
        task->SetPriorTask(get_task); 
		job_->AddTask(get_task, true);
		DLOG(INFO)<<"MutateSession::Init(), should check value or version, \
                    create a read task";
        DLOG(INFO)<<"MutateSession::Init() GetTask id="<<get_task->Seq();
	}else{
		DLOG(INFO)<<"MutateSession::Init() no need to check version or value";
		job_->AddTask(task, true);
        DLOG(INFO)<<"MutateSession::Init() MutateTask id="<<task->Seq();
	}
	//task->SetPriorTask(get_task); 
    *next = kWait;
}

void MutateSession::Emit(Payload* response, Action* next){
    *next = kDone;
    *response = response_;
    DLOG(INFO)<<"MutateSession::Emit() response size="<<response->body().size()
        <<" channel name="<<channel()->name();
}

void MutateSession::Done(Code* code){
    *code = 0;
    DLOG(INFO)<<"MutateSession::Done()"
        <<" channel name="<<channel()->name();
    delete job_;
}

void MutateSession::Cancel() {
    DLOG(INFO)<<"MutateSession::Cancel()"
        <<" channel name="<<channel()->name();
}

void ListSession::Init(const Payload& request, Action* next){
    buffer_ = request;
	ListMessageS *msg = new ListMessageS;
	bool ret = msg->ParseFrom(request.body().data(), request.body().size());
	if (!ret){
		LOG(ERROR)<<"ListSession::Init() error len=%d"
            <<request.body().size();
        channel()->Close();
        delete msg;
        return ;
	}
	DLOG(INFO)<<"ListSession::Init() get a payload, size= "
                <<msg->key_.ToString()
				<<request.body().size()<<" u16Ns_ "<<msg->u16Ns_;
	job_ = new ListJob(boost::static_pointer_cast<ListSession>(shared_from_this()));
	ListTask* task = new ListTask(msg, kTypeRead, 
								  job_, config_manager_->GetDb());
	task_vec_.push_back(task);
	job_->AddTask(task, true);
    DLOG(INFO)<<"ListSession::Init() ListTask id="<<task->Seq();
    *next = kWait;
}

void ListSession::Emit(Payload* response, Action* next){
    *next = kWait;
    *response = response_;
    DLOG(INFO)<<"ListSession::Emit() response size="<<response->body().size();
}

void ListSession::Done(Code* code){
    *code = 0;
    DLOG(INFO)<<"ListSession::Done()";
    delete job_;
}

void ListSession::Cancel() {
    DLOG(INFO)<<"CheckSession::Cancel()";   //todo should add Cancel() in other session, to notify Done()
}

void AtomicIncrSession::Init(const Payload& request, Action* next){
	buffer_ = request;
	AtomicIncrMessageS *msg = new AtomicIncrMessageS;
	bool ret = msg->ParseFrom(request.body().data(), request.body().size());
	if (!ret){
		LOG(ERROR)<<"AtomicIncrSession::Init() error len="
            <<request.body().size();
        channel()->Close();
        delete msg;
        return ;
	}
	DLOG(INFO)<<"AtomicIncrSession::Init() get a payload, size= "
                <<msg->key_.ToString()
				<<request.body().size()<<" u16Ns_ "<<msg->u16Ns_;
	job_ = new Job(boost::static_pointer_cast<AtomicIncrSession>(shared_from_this()));
	if (config_manager_->IsMaster()){
		msg->u8from_ = config_manager_->GetFromTypeofMaster();
	}
	AtomicIncrTask* task = new AtomicIncrTask(config_manager_, 
                                                msg,request, kTypeWrite, 
											  job_, config_manager_->GetDb());
	task_vec_.push_back(task);

	GetTask* get_task = NULL;
	{
		GetMessageS * get_msg = new GetMessageS;
		get_msg->u16Ns_		  = msg->u16Ns_;
		get_msg->key_		  = msg->key_;
		get_msg->u16BucketID_  = msg->u16BucketID_;
		get_task			  = new GetTask(get_msg, kTypeReadSubTask, 
											job_, config_manager_->GetDb());
		task_vec_.push_back(get_task);
		get_task->SetNextTask(task);
		job_->AddTask(get_task, true);
        DLOG(INFO)<<"AtomicIncrSession::Init() GetTask id="<<get_task->Seq();
	}
	task->SetPriorTask(get_task);
    *next = kWait;
}

void AtomicIncrSession::Emit(Payload* response, Action* next){
    *next = kDone;
    *response = response_;
    DLOG(INFO)<<"AtomicIncrSession::Emit() response size="
                    <<response->body().size();
}

void AtomicIncrSession::Done(Code* code){
    *code = 0;
    DLOG(INFO)<<"AtomicIncrSession::Done()";
    delete job_;
}

void AtomicIncrSession::Cancel() {
    DLOG(INFO)<<"CheckSession::Cancel()";
}

void AtomicAppendSession::Init(const Payload& request, Action* next){
    buffer_ = request;
    AtomicAppendMessageS *msg = new AtomicAppendMessageS;
    bool ret = msg->ParseFrom(request.body().data(), request.body().size());
    if (!ret){
	    LOG(ERROR)<<"AtomicAppendSession::Init() error len=%d"
            <<request.body().size();
        channel()->Close();
        delete msg;
        return ;
    }
    DLOG(INFO)<<"AtomicAppendSession::Init() get a payload, size= "
                <<msg->key_.ToString()
			    <<request.body().size()<<" u16Ns_ "<<msg->u16Ns_
                <<" channel name="<<channel()->name();
    job_ = new Job(boost::static_pointer_cast<AtomicAppendSession>(shared_from_this()));
    if (config_manager_->IsMaster()){
	    msg->u8from_ = config_manager_->GetFromTypeofMaster();
    }
    AtomicAppendTask* task = new AtomicAppendTask(config_manager_, msg, 
                                                  request,kTypeWrite, 
											      job_, 
                                                  config_manager_->GetDb());
    task_vec_.push_back(task);
    GetTask* get_task = NULL;
    {
	    GetMessageS * get_msg = new GetMessageS;
	    get_msg->u16Ns_		  = msg->u16Ns_;
	    get_msg->key_		  = msg->key_;
	    get_msg->u16BucketID_  = msg->u16BucketID_;
	    get_task			  = new GetTask(get_msg, kTypeReadSubTask, 
										    job_, config_manager_->GetDb());
	    task_vec_.push_back(get_task);
	    get_task->SetNextTask(task);
	    job_->AddTask(get_task, true);
        DLOG(INFO)<<"AtomicAppendSession::Init() GetTask id="<<get_task->Seq();
    }
    task->SetPriorTask(get_task);
    *next = kWait;
}

void AtomicAppendSession::Emit(Payload* response, Action* next){
    *next = kDone;
    *response = response_;
    DLOG(INFO)<<"AtomicAppendSession::Emit() response size="
                <<response->body().size();
}

void AtomicAppendSession::Done(Code* code){
    *code = 0;
    DLOG(INFO)<<"AtomicAppendSession::Done()";
    delete job_;
}

void AtomicAppendSession::Cancel() {
    DLOG(INFO)<<"CheckSession::Cancel()";
}


///************************************************************************/
///* request from store to store                                          */
///************************************************************************/
GetVersionSession::~GetVersionSession(){
	DLOG(INFO)<<"GetVersionSession::~GetVersionSession()";
}
void GetVersionSession::Init(const Payload& request, Action* next){
    buffer_ = request;
	LOG(INFO)<<"GetVersionSession::Init(), buffer length= "
                    <<request.body().size();
	GetVersionMessageS *msg = new GetVersionMessageS;
	bool ret = msg->ParseFrom(request.body().data(), request.body().size());
	if (!ret){
		LOG(ERROR)<<"GetVersionSession::Init() error len=%d"
                        <<request.body().size();
        channel()->Close();
        delete msg;
        return ;
	}
	LOG(INFO)<<"GetVersionSession::Init() get a payload, size= "
				<<request.body().size()<<" address="<<msg->host_port_.ToString();
	job_ = new Job(boost::static_pointer_cast<GetVersionSession>(shared_from_this()));
	Slice home_port = config_manager_->GetHomeHostPort();
	GetVersionTask* task = new GetVersionTask(home_port, msg, kTypeRead, 
											  job_, config_manager_->GetDb());
	task_vec_.push_back(task);
	job_->AddTask(task, true);
    LOG(INFO)<<"GetVersionSession::Init() GetVersionTask id="<<task->Seq();
    *next = kWait;
}

void GetVersionSession::Emit(Payload* response, Action* next){
    *next = kDone;
    *response = response_;
    LOG(INFO)<<"GetVersionSession::Emit()  response size="
                <<response->body().size();
}

void GetVersionSession::Done(Code* code){
    *code = 0;
    LOG(INFO)<<"GetVersionSession::Done()";
    delete job_;
}

void GetVersionSession::Cancel() {
    LOG(INFO)<<"GetVersionSession::Cancel()";
}

void MutationPrepareSession::Init(const Payload& request, Action* next){
	buffer_ = request;    // todo 解压缩包里面 好像拷贝了一个新的buffer， 跟踪查看一下
	DLOG(INFO)<<"MutationPrepareSession::Init(), buffer length= "
                    <<request.body().size();
	ldd::protocol::PrepareMessageS pm;
	bool ret = pm.ParseFrom(request.body().data(), request.body().size());
	if (!ret){
		LOG(ERROR)<<"MutationPrepareSession::Init() error len=%d"
            <<request.body().size();
        channel()->Close();
        return ;
	}
	DLOG(INFO)<<"MutationPrepareSession::Init() get a payload, size= "
                    <<request.body().size();
    host_port_ = pm.home_port_.ToString();
    *next = kEmit;
}

void MutationPrepareSession::Emit(Payload* response, Action* next){
    *next = kDone;

    ldd::protocol::PrepareMessageS out;
    out.home_port_				= host_port_;
    out.dest_host_port_			= config_manager_->GetHomeHostPort();
    DLOG(INFO)<<"MutationPrepareSession::Init() repond home_port="
        <<out.home_port_.ToString()<<" dest_adress="
        <<out.dest_host_port_.ToString();
    int size = out.MaxSize();
    Buffer buffer;
    buffer.Reset(size);
    size_t real_len = out.SerializeTo(buffer.ptr());
    buffer.ReSize(real_len);
    DLOG(INFO)<<"MutationPrepareSession::Emit() repsond here payload size="
        <<real_len;
    response->SetBody(buffer);
}

void MutationPrepareSession::Done(Code* code){
    *code = 0;
    DLOG(INFO)<<"MutationPrepareSession::Done()";
}

void MutationPrepareSession::Cancel() {
    DLOG(INFO)<<"MutationPrepareSession::Cancel()";
}


}//namespace storage
}//namespace ldd


