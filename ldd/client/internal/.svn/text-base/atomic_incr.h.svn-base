#ifndef __LDD_CLIENT_RAW_ATOMIC_INCR__H_
#define __LDD_CLIENT_RAW_ATOMIC_INCR__H_


#include "ldd/net/outgoing_msg.h"

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/client/response.h"

#include "client_impl.h"

using namespace ldd::net;

namespace ldd {
namespace client {
namespace raw {
                                 
class AtomicIncrRequest : 
        public TypedOutgoingMsg<LDD_CLIENT_2_PROXY_ATOMIC_INCR>{
public:
	AtomicIncrRequest(ClientImpl* ct, const std::string& ns,
                    const std::string& key,
		            int32_t s32operand, int32_t	s32initial,
		            uint64_t u64ttl, const CasCompletion& completion, 
		            const std::string& using_host, int using_port,
		            RedoPolicy* rp);

    virtual ~AtomicIncrRequest() {}
    
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
	ClientImpl*			client_;
	std::string			ns_;
	std::string			key_;
	int32_t				s32operand_;
	int32_t				s32initial_;
	uint64_t			u64ttl_;
	CasCompletion		cas_completion_;
	std::string			using_host_;
	int					using_port_;
	RedoPolicy*			rp_;

	protocol::CasResponse response_;
};



} // namespace ldd {
} // namespace client {
} // namespace raw {


#endif 

