

#include "session.h"
#include "job.h"
#include "task.h"
#include "task_queue.h"
#include "ldd/net/buffer.h"


using namespace ldd::net;

namespace ldd{
namespace storage{


Job::Job(boost::shared_ptr<Session>msg) : session_(msg), task_num_(0){

}


void Job::AddResult(ldd::protocol::Response* response){
	//task_num_--;
	DLOG(INFO)<<"Job::AddResult() remaining  task_num="<<task_num_;
	int len = response->MaxSize();
	Buffer buffer;
	buffer.Reset(len);		
	size_t real_len = response->SerializeTo(buffer.ptr());
	DLOG(INFO)<<"Job::AddResult() response  buffer len="<<len;
	buffer.ReSize(real_len);

	//if(task_num_ == 0){
        session_->SetBody(buffer);
	//}
}

Job::~Job(){
    DLOG(INFO)<<"Job::~Job()";
}

void Job::AddTask(Task* task, bool count_task){
	if (count_task && 
		(task->GetTypeTask()    ==kTypeRead 
		|| task->GetTypeTask() ==kTypeWrite )){
			task_num_++;
			DLOG(INFO)<<"Job::AddTask() add count number="<<task_num_;
	}

	if(task->GetTypeTask() == kTypeRead  || 
        task->GetTypeTask() ==kTypeReadSubTask){
		DLOG(INFO)<<"Job::AddTask() add a kTypeRead or kTypeReadSubTask task, type= "
			<<task->GetTypeTask()<<" count="<<count_task;
		SingletonHolder<ReadTaskQueue>::Instance().push_back(task);
	}else if(task->GetTypeTask() == kTypeWrite || 
            task->GetTypeTask() == kTypeWriteSubTask){
		DLOG(INFO)<<"Job::AddTask() add a kTypeWrite or kTypeWriteSubTask task, type= "
			<<task->GetTypeTask()<<" count="<<count_task;
		SingletonHolder<WriteTaskQueue>::Instance().push_back(task);
	}	
}

void Job::RescheduleTask(Task* task, bool count_task){
    if(task->GetTypeTask() == kTypeRead  || 
        task->GetTypeTask() ==kTypeReadSubTask){
            DLOG(INFO)<<"Job::RescheduleTask() readd a kTypeRead task, type= "
                <<task->GetTypeTask()<<" count="<<count_task<<" task_id="<<task->Seq();
            SingletonHolder<ReadTaskQueue>::Instance().push_front(task);
    }else if(task->GetTypeTask() == kTypeWrite || 
        task->GetTypeTask() == kTypeWriteSubTask){
            DLOG(INFO)<<"Job::RescheduleTask() readd a kTypeWrite task, type= "
                <<task->GetTypeTask()<<" count="<<count_task<<" task_id="<<task->Seq();
            SingletonHolder<WriteTaskQueue>::Instance().push_front(task);
    }	
}


void ListJob::AddResult(ldd::protocol::Response* response){
	protocol::GetOrListResponse* gp = 
            static_cast<protocol::GetOrListResponse*>(response);
	task_num_--;
	DLOG(INFO)<<"ListJob::AddResult() remaining  task_num="<<task_num_;
	int len = gp->MaxSize();
	Buffer buffer;
	buffer.Reset(len);
	size_t real_len = gp->SerializeTo(buffer.ptr());
	buffer.ReSize(real_len);
    session_->SetBody(buffer);
	DLOG(INFO)<<"ListJob::AddResult() response  buffer len="<<len;
}

void ListJob::Done(){
    session_->Done();
}

}//namespace storage
}//namespace ldd


