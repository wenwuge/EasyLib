#ifndef __LDD_CLIENT_RAW_CHECK__H_
#define __LDD_CLIENT_RAW_CHECK__H_


#include "ldd/net/outgoing_msg.h"

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/client/response.h"

#include "client_impl.h"
namespace ldd {
namespace client {
namespace raw {

using namespace ldd::net;

class CheckRequest : public TypedOutgoingMsg<LDD_CLIENT_2_PROXY_CHECK>{
public:
	CheckRequest(ClientImpl* ct, const std::string&ns, const std::string& key, 
				const CheckCompletion& complete, 
				const std::string& using_host,
				int				   using_port,
				RedoPolicy* rp);
    virtual ~CheckRequest() {}

    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);


    ////以下逻辑由用户编写，由框架调用
    //virtual void SerializeTo(Buffer* request,
				//			std::map<int8_t, Buffer>* extras);

    ////called by logic
    //virtual void HandleResult(const Buffer &buff, 
				//			const std::map<int8_t, Buffer>& extras); 

    //virtual void HandleFinalize();
    //virtual void HandleTimeout();
    //virtual void HandleError();

private:
	ClientImpl*			client_;
	std::string			ns_;
	std::string			key_;
	CheckCompletion		check_completion_;
	std::string			using_host_;
	int					using_port_;
	RedoPolicy*			rp_;

    protocol::CheckResponse response_;
};


} // namespace ldd {
} // namespace client {
} // namespace raw {



#endif 

