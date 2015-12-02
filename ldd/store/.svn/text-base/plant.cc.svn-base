#include "plant.h"
#include "task.h"
#include "task_queue.h"




namespace ldd{
namespace storage{


RequestProcessPlant::RequestProcessPlant(): stop_(false){	

}
RequestProcessPlant::~RequestProcessPlant(){
	LOG(ERROR)<<"RequestProcessPlant::~RequestProcessPlant()";
	while (stop_.Load()){
		usleep(0);
		LOG(ERROR)<<"usleep(0)";
	}
    LOG(ERROR)<<"RequestProcessPlant::~RequestProcessPlant() true exit";
}
void RequestProcessPlant::run(){
	while(!stop_.Load()){
		schedule();
	}
	LOG(ERROR)<<"RequestProcessPlant::run() Stopped!";
	stop_ = false;
}

void RequestProcessPlant::Stop(){
    LOG(ERROR)<<"RequestProcessPlant::Stop()";
	stop_ = true; 
}

ReadRequestProcessPlant::~ReadRequestProcessPlant(){
	LOG(ERROR)<<"ReadRequestProcessPlant::~ReadRequestProcessPlant()!";
	SingletonHolder<ReadTaskQueue>::Instance().Destory();
};

void ReadRequestProcessPlant::schedule(){
	DLOG(INFO)<<"ReadRequestProcessPlant::schedule()";
	Task *task = SingletonHolder<ReadTaskQueue>::Instance().pop_front();
	if (task){
		task->Run();	
	}
}

WriteRequestProcessPlant::~WriteRequestProcessPlant(){
	LOG(ERROR)<<"WriteRequestProcessPlant::~WriteRequestProcessPlant()!";
	SingletonHolder<WriteTaskQueue>::Instance().Destory();
};
void WriteRequestProcessPlant::schedule(){		
	DLOG(INFO)<<"WriteRequestProcessPlant::schedule()";
	Task *task = SingletonHolder<WriteTaskQueue>::Instance().pop_front();
	if (task){
		task->Run();	
	}
}



}//namespace storage
}//namespace ldd


