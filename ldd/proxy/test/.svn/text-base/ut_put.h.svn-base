#include "ldd/net/outgoing_msg.h"

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/client/response.h"

using namespace ldd::net;

class Put : 
    public TypedOutgoingMsg<LDD_CLIENT_2_PROXY_MUTATE>{
public:
    Put(const ldd::protocol::MutateMessage &req) : req_(req) {}
    virtual ~Put() {}


protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
    ldd::protocol::MutateMessage req_;
    ldd::protocol::MutateResponse res_;
};

