#ifndef __LDD_CLIENT_RAW_LIST__H_
#define __LDD_CLIENT_RAW_LIST__H_

#include "ldd/net/outgoing_msg.h"
#include "ldd/util/mutex.h"
#include <sys/time.h>

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/client/response.h"

#include "client_impl.h"

namespace ldd {
namespace client {
namespace raw {

using namespace ldd::net;

class ListRequest : public TypedOutgoingMsg<LDD_CLIENT_2_PROXY_LIST> {
public:
	ListRequest(ClientImpl* ci, const std::string& ns, const std::string& key,
				uint32_t u32limit, uint8_t	u8position,  
				const DataCompletion& completion, 
				const std::string& using_host,
				int					using_port,
				RedoPolicy* rp);
    virtual ~ListRequest() {}

    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
	ClientImpl*		client_;
	std::string		ns_;
	std::string		key_;
	uint32_t		u32limit_;
	uint8_t			u8position_;  
	DataCompletion	completion_;
	std::string		using_host_;
	int				using_port_;
	RedoPolicy*		rp_;

    protocol::GetOrListResponse response_;
};


} // namespace ldd {
} // namespace client {
} // namespace raw {

#endif 

