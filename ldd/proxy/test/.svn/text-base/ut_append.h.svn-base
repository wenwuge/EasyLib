#include "ldd/net/outgoing_msg.h"

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/client/response.h"

using namespace ldd::net;

class Append  : 
    public TypedOutgoingMsg<LDD_CLIENT_2_PROXY_ATOMIC_APPEND>{
public:
    Append(const ldd::protocol::AtomicAppendMessage &req) : req_(req) {}
    virtual ~Append() {}

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
    ldd::protocol::AtomicAppendMessage req_;
    ldd::protocol::CasResponse res_;
};

