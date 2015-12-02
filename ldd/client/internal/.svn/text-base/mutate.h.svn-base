
#ifndef __LDD_CLIENT_RAW_MUTATE__H_
#define __LDD_CLIENT_RAW_MUTATE__H_

#include "ldd/net/outgoing_msg.h"

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/server/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/server/store_proto.h"
#include "ldd/protocol/client/response.h"

#include "client_impl.h"

namespace ldd {
namespace client {
namespace raw {

using namespace ldd::net;

class MutateRequest : public TypedOutgoingMsg<LDD_CLIENT_2_PROXY_MUTATE> {
public:
		MutateRequest(ClientImpl* ct, const Mutation& m, 
		const MutateCompletion& completion,
		const std::string& using_host,
		int				   using_port,
		RedoPolicy* rp) ;

    virtual ~MutateRequest() {}

    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);


private:
	ClientImpl*			 client_;

	std::string			 name_space_;
	std::string			 key_;						
	std::string			 expected_value_;			
	std::string			 value_;					

	MutateCompletion	 completion_;
	std::string			 using_host_;
	int					 using_port_;
	RedoPolicy*			 rp_;

    protocol::MutateMessage  request_;
    protocol::MutateResponse response_;

};


} // namespace ldd {
} // namespace client {
} // namespace raw {


#endif 

