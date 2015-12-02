#ifndef __LDD__STORAGE_CONTEXT_H_
#define __LDD__STORAGE_CONTEXT_H_

#include <stdint.h>
#include "ldd/net/payload.h"
#include "ldd/protocol/server/command_type.h"
#include "ldd/net/outgoing_msg.h"

namespace ldd{
	namespace protocol{
		class PrepareMessageS;
		class MutateMessageS;
		class Message;
		class AtomicIncrMessageS;
		class AtomicAppendMessageS;
        class MutateResponse;
	}
}


namespace ldd{
namespace storage{

using namespace ldd::net;
using ldd::protocol::PrepareMessageS;
using ldd::protocol::Message;
using ldd::protocol::MutateMessageS;
using ldd::protocol::AtomicIncrMessageS;
using ldd::protocol::AtomicAppendMessageS;
using ldd::protocol::MutateResponse;

class ConfigManager;
class ModifyBaseTask;
class Job;

class MutateContext : 
        public TypedOutgoingMsg<LDD_PROXY_2_STORE_MUTATE>{
public:
	MutateContext(ConfigManager* cm, Job* job, Message* gms, 
                    const net::Payload& payload,
                    int recv_timeout, int done_timeout);
	virtual ~MutateContext();

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
    ConfigManager* cm_;
    Job* job_;
	Message* gms_;
	net::Payload payload_;
    net::Buffer buffer_;
    int recv_timeout_;
    int done_timeout_;
    int64_t s64Version_;
};

class AtomicIncrContext : 
        public TypedOutgoingMsg<LDD_PROXY_2_STORE_ATOMIC_INCR>{
public:
    AtomicIncrContext(ConfigManager* cm, Job* job, Message* gms, 
                    const net::Payload& payload,
                    int recv_timeout, int done_timeout) : 
                    cm_(cm),
                    job_(job),
                    gms_(gms), payload_(payload),
                    recv_timeout_(recv_timeout),done_timeout_(done_timeout) {

	};
	virtual ~AtomicIncrContext();

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
    ConfigManager* cm_;
    Job* job_;
	Message* gms_;
	net::Payload payload_;
    int recv_timeout_;
    int done_timeout_;
};

class AtomicAppendContext : 
            public TypedOutgoingMsg<LDD_PROXY_2_STORE_ATOMIC_APPEND>{
public:
    AtomicAppendContext(ConfigManager* cm, Job* job, Message* gms, 
                        const net::Payload& payload,
                        int recv_timeout, int done_timeout) :
                        cm_(cm),
                        job_(job),
                        gms_(gms), payload_(payload),
                        recv_timeout_(recv_timeout),
                        done_timeout_(done_timeout){

	};
	virtual ~AtomicAppendContext();

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
    ConfigManager* cm_;
    Job* job_;
	Message* gms_;
	net::Payload payload_;
    int recv_timeout_;
    int done_timeout_;
};

class ContextFactory{
public:
    static boost::shared_ptr<net::OutgoingMsg> MakeContext(
                                    ConfigManager* cm,
                                    Job* job, 
                                    int type, 
                                    protocol::Message* msg, 
                                    const net::Payload& payload, 
                                    int recv_timeout,
                                    int done_timeout);
private:
	ContextFactory();
	ContextFactory(const ContextFactory&);
	ContextFactory& operator=(const ContextFactory&);
};


class GetVersionContext : 
        public TypedOutgoingMsg<LDD_STORE_2_STORE_GET_VERSION>{
public:
	GetVersionContext(ConfigManager* m, const std::string&host_port)
		: manager_(m), host_port_(host_port){

	};
	virtual ~GetVersionContext();

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
	ConfigManager*	manager_;
	std::string		host_port_;		// todo replace by slice
	int64_t			version;
};

class PrepareResultH;
class PrepareContext : 
        public TypedOutgoingMsg<LDD_STORE_2_STORE_MUTATION_PREPARE>{
public:
	PrepareContext(PrepareResultH*	resultH, PrepareMessageS* pm, uint64_t& version)
		: prepare_result_(resultH), pm_(pm), version_(version){

	};
	virtual ~PrepareContext();

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
	PrepareResultH*	 prepare_result_;
	PrepareMessageS* pm_;
    uint64_t version_;

};

class PrepareResultH{
public:
	PrepareResultH(ConfigManager* config_manager, ModifyBaseTask* task) : 
	  config_manager_(config_manager), task_(task), 
	  result_num_(0), enable_write(true){
	}
	bool SendPrepareMessage(uint64_t& version);
	bool AddResult(bool right);
    int get_done_timeout();
    int get_recv_timeout();

private:
	ConfigManager* config_manager_;
	ModifyBaseTask* task_;
	int   result_num_;
	bool  enable_write;
};


class SynWriteMessageHandle{
public:
	SynWriteMessageHandle(ConfigManager* config_manager) :
	  config_manager_(config_manager){
	  }
	  ~SynWriteMessageHandle(){};
	
	  bool SynToSlaves(Job* job, int type, 
                        Message* msg , const net::Payload& payload);
private:
	ConfigManager* config_manager_;
};



}//namespace storage
}//namespace ldd





#endif //__LDD__STORAGE_CONTEXT_H_
