

#ifndef __LDD__STORAGE_TASK_H_
#define __LDD__STORAGE_TASK_H_


#include <stdint.h>
#include <string>
#include "ldd/util/slice.h"
#include "ldd/net/payload.h"

namespace ldd{
namespace protocol{
		class GetVersionMessageS;
		class GetMessageS;
		class CheckMessageS;
		class MutateMessageS;
		class ListMessageS;
		class AtomicIncrMessageS;
		class AtomicAppendMessageS;
		class Response;
	}
}


namespace ldd{
namespace storage{

using ldd::util::Slice;
using ldd::protocol::GetVersionMessageS;
using ldd::protocol::GetMessageS;
using ldd::protocol::CheckMessageS;
using ldd::protocol::MutateMessageS;
using ldd::protocol::ListMessageS;
using ldd::protocol::AtomicIncrMessageS;
using ldd::protocol::AtomicAppendMessageS;
using ldd::protocol::Response;


enum TaskType{
	kTypeRead = 1,
	kTypeWrite,
	kTypeReadSubTask,
	kTypeWriteSubTask
};

class Job;
class ConfigManager;
class DB;

class Task{
public:
	Task(TaskType type, Job* job, DB* db) 
		: type_(type), job_(job), db_(db), 
			next_(NULL), prior_(NULL),task_id_(0){

	}
	virtual	~Task();
	virtual void Execute() = 0;
	virtual void SetNextTask(Task* task);
	virtual void SetPriorTask(Task* task);
			void Run();
			void ReSchedule();
			TaskType GetTypeTask();
            void SetSeq(int seq){
                task_id_ = seq;
            }
            
            int Seq(){
                return task_id_;
            }
protected:
	TaskType	type_;
	Job*		job_;
	DB*			db_;
	Task*		next_;
	Task*		prior_;
    int         task_id_;
};

class GetTask : public Task{
public:
	GetTask(GetMessageS* msg, TaskType type, Job* job, DB* db)
		: Task(type, job, db), msg_(msg), reponse_(NULL){

	}
	~GetTask();
public:
	virtual void Execute();
	Response* GetResponse(){
		return reponse_;
	};

private:
	GetMessageS*	msg_;
	std::string		value_;
	Response*		reponse_;
};


class CheckTask : public Task{
public:
	CheckTask(CheckMessageS* msg, TaskType type, Job* job, DB* db)
		: Task(type, job, db), msg_(msg){

	}
	~CheckTask();
public:
	virtual void Execute();

private:
	CheckMessageS*	 msg_;
};

class PrepareResultH;
class SynWriteMessageHandle;
class ModifyBaseTask : public Task {
public:
	ModifyBaseTask(const net::Payload& payload, TaskType type, 
					Job* job, DB* db, 
					ConfigManager*  config_manager) 
	  : Task(type, job, db), /*payload_(payload),*/ config_manager_(config_manager), 
	  prepare_handle_(NULL),syn_handle_(NULL),version_(0),
	  prepare_checked_(false),can_mutate_(false){
          payload_.SetBody(payload.body());
	}
	~ModifyBaseTask();
	void	enable_mutate(bool yes){ 
		can_mutate_			= yes;
	};
	bool prepared(){
		return prepare_checked_;
	}
	void prepare_checked(){
		prepare_checked_ = true;
	}
	bool can_mutate(){
		return can_mutate_;
	}

	virtual void Execute();
	virtual void DoExecute() = 0;
	virtual int64_t GetSynVersion() = 0;
	virtual uint8_t GetMsgFrom()	= 0;

protected:
	net::Payload					payload_;
	ConfigManager*			config_manager_;
	PrepareResultH*			prepare_handle_;
	SynWriteMessageHandle*  syn_handle_;
	uint64_t				version_;
	
private:
	bool			prepare_checked_;
	bool			can_mutate_;
};


class MutateTask : public ModifyBaseTask{
public:
	MutateTask(ConfigManager* config_manager, MutateMessageS* msg, 
				const net::Payload& payload, 
				TaskType type, Job* job, DB* db)
		: ModifyBaseTask(payload, type, job, db, config_manager), msg_(msg){

	}
	~MutateTask();
public:
	virtual void DoExecute();
	virtual int64_t GetSynVersion();
	virtual uint8_t GetMsgFrom();

private:
	MutateMessageS* msg_;
	std::string		value_;
	
};


class ListTask : public Task{
public:
	ListTask(ListMessageS* msg, TaskType type, Job* job, DB* db)
		: Task(type, job, db), msg_(msg){

	}
	~ListTask();
public:
	virtual void Execute();
private:
	ListMessageS*	msg_;
};


class AtomicIncrTask : public ModifyBaseTask{
public:
	AtomicIncrTask(ConfigManager* config_manager, AtomicIncrMessageS* msg, 
					const net::Payload& payload, 
					TaskType type, Job* job, DB* db)
		: ModifyBaseTask(payload, type, job,db, config_manager), msg_(msg){

	}
	~AtomicIncrTask();
public:
	virtual void DoExecute();
	virtual int64_t GetSynVersion();
	virtual uint8_t GetMsgFrom();

private:
	AtomicIncrMessageS* msg_;
};


class AtomicAppendTask : public ModifyBaseTask{
public:
	AtomicAppendTask(ConfigManager* config_manager, AtomicAppendMessageS* msg, 
						const net::Payload& payload, 
						TaskType type, Job* job, DB* db)
		:ModifyBaseTask(payload, type, job, db, config_manager), msg_(msg){

	}
	~AtomicAppendTask();
public:
	virtual void DoExecute();
	virtual int64_t GetSynVersion();
	virtual uint8_t GetMsgFrom();

private:
	AtomicAppendMessageS*	msg_;
	std::string				new_value_;
};


class GetVersionTask : public Task{
public:
	GetVersionTask(Slice home_port, GetVersionMessageS* msg, 
					TaskType type, Job* job, DB* db)
		: Task(type, job, db), home_port_(home_port), msg_(msg){
	
	}
	~GetVersionTask();
public:
	virtual void Execute();

private:
	Slice					home_port_;
	GetVersionMessageS*		msg_;
	std::string				value_;
};


}//namespace storage
}//namespace ldd





#endif //__LDD__STORAGE_TASK_H_
