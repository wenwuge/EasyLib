#include "ldd/net/outgoing_msg.h"

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/client/response.h"

using namespace ldd::net;

class Check : 
    public TypedOutgoingMsg<LDD_CLIENT_2_PROXY_CHECK>{
public:
    Check(const ldd::protocol::CheckMessage &req) : req_(req) {}
    virtual ~Check() {}

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);


private:
    ldd::protocol::CheckMessage req_;
    ldd::protocol::CheckResponse res_;
};

