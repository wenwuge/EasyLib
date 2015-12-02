
#ifndef __LDD__STORAGE_SESSION_H_
#define __LDD__STORAGE_SESSION_H_

#include <map>
#include "ldd/net/incoming_msg.h"
#include "ldd/net/buffer.h"
#include "ldd/protocol/server/store_proto.h"
#include "ldd/protocol/server/command_type.h"

using namespace std;
using namespace ldd::net;


namespace ldd{
namespace storage{

class Task;
class Job;
class ConfigManager;

class Session : public IncomingMsg{
public:
	Session() : job_(NULL){
	};

    ~Session();
    void SetBody(net::Buffer buf);
    void Done();
protected:
	Job*				job_;
	std::vector<Task*>	task_vec_;
	Payload				buffer_;
    Payload             response_;
};

class ReadSession : public Session{
public:
	ReadSession(ConfigManager* config) : config_manager_(config){

	}
	virtual ~ReadSession(){

	};

    static const Type kType = LDD_PROXY_2_STORE_GET;
    Type type() const { return kType; }


protected:
    virtual void Init(const Payload& request, Action* next) ;
    virtual void Emit(Payload* response, Action* next) ;
    virtual void Done(Code* code) ;
    virtual void Cancel() ;

private:
	ConfigManager* config_manager_;
};

class CheckSession : public Session{
public:
	CheckSession(ConfigManager* config) : config_manager_(config){

	}
	virtual ~CheckSession(){

	}

    static const Type kType = LDD_PROXY_2_STORE_CHECK;
    Type type() const { return kType; }

protected:
    virtual void Init(const Payload& request, Action* next) ;
    virtual void Emit(Payload* response, Action* next) ;
    virtual void Done(Code* code) ;
    virtual void Cancel() ;

private:
	ConfigManager* config_manager_;
};

class MutateSession : public Session{
public:
	MutateSession(ConfigManager* config) : config_manager_(config){

	}
	virtual ~MutateSession(){

	}

    static const Type kType = LDD_PROXY_2_STORE_MUTATE;
    Type type() const { return kType; }

protected:
    virtual void Init(const Payload& request, Action* next) ;
    virtual void Emit(Payload* response, Action* next) ;
    virtual void Done(Code* code) ;
    virtual void Cancel() ;

private:
	ConfigManager* config_manager_;

};

class ListSession : public Session{
public:
	ListSession(ConfigManager* config) : config_manager_(config){

	}
	virtual ~ListSession(){

	}

    static const Type kType = LDD_PROXY_2_STORE_LIST;
    Type type() const { return kType; }

protected:
    virtual void Init(const Payload& request, Action* next) ;
    virtual void Emit(Payload* response, Action* next) ;
    virtual void Done(Code* code) ;
    virtual void Cancel() ;

private:
	ConfigManager* config_manager_;
};

class AtomicIncrSession : public Session{
public:
	AtomicIncrSession(ConfigManager* config):config_manager_(config){

	}
	virtual ~AtomicIncrSession(){

	}

    static const Type kType = LDD_PROXY_2_STORE_ATOMIC_INCR;
    Type type() const { return kType; }

protected:
    virtual void Init(const Payload& request, Action* next) ;
    virtual void Emit(Payload* response, Action* next) ;
    virtual void Done(Code* code) ;
    virtual void Cancel() ;
private:
	ConfigManager* config_manager_;
};

class AtomicAppendSession : public Session{
public:
	AtomicAppendSession(ConfigManager* config) : config_manager_(config){

	}
	virtual ~AtomicAppendSession(){

	}

    static const Type kType = LDD_PROXY_2_STORE_ATOMIC_APPEND;
    Type type() const { return kType; }


protected:
    virtual void Init(const Payload& request, Action* next) ;
    virtual void Emit(Payload* response, Action* next) ;
    virtual void Done(Code* code) ;
    virtual void Cancel() ;


private:
	ConfigManager* config_manager_;
};

/************************************************************************/
/* request from store to store                                          */
/************************************************************************/
class GetVersionSession : public Session{
public:
	GetVersionSession(ConfigManager* config) : config_manager_(config){

	}
	virtual ~GetVersionSession();

    static const Type kType = LDD_STORE_2_STORE_GET_VERSION;
    Type type() const { return kType; }

protected:
    virtual void Init(const Payload& request, Action* next) ;
    virtual void Emit(Payload* response, Action* next) ;
    virtual void Done(Code* code) ;
    virtual void Cancel() ;

private:
	ConfigManager* config_manager_;
};

class MutationPrepareSession : public Session{
public:
	MutationPrepareSession(ConfigManager* config) : config_manager_(config){

	}
	virtual ~MutationPrepareSession(){

	}

    static const Type kType = LDD_STORE_2_STORE_MUTATION_PREPARE;
    Type type() const { return kType; }

protected:
    virtual void Init(const Payload& request, Action* next) ;
    virtual void Emit(Payload* response, Action* next) ;
    virtual void Done(Code* code) ;
    virtual void Cancel() ;

private:
	ConfigManager* config_manager_;
    std::string host_port_;
};



}//namespace storage
}//namespace ldd







#endif //__LDD__STORAGE_SESSION_H_
