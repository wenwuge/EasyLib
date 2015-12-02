#ifndef __LDD_CLIENT_RAW_ATOMIC_APPEND__H_
#define __LDD_CLIENT_RAW_ATOMIC_APPEND__H_

#include "ldd/net/outgoing_msg.h"

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/client/response.h"
#include "client_impl.h"


namespace ldd {
namespace client {
namespace raw {

using namespace ldd::net;

class AtomicAppendRequest : 
            public TypedOutgoingMsg<LDD_CLIENT_2_PROXY_ATOMIC_APPEND> {
public:
	AtomicAppendRequest(ClientImpl* ct, const std::string& ns, 
                        const std::string& key, 
						uint8_t u8position, const std::string& content, 
						uint64_t u64ttl, const CasCompletion& completion,
						const std::string& using_host, int using_port,
						RedoPolicy* rp);

    virtual ~AtomicAppendRequest() {}


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
	uint8_t				u8position_;
	std::string			content_;
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




