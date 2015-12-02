#include "ldd/net/outgoing_msg.h"
#include "ldd/util/mutex.h"
#include <sys/time.h>

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/client/response.h"

using namespace ldd::net;

class List : 
    public TypedOutgoingMsg<LDD_CLIENT_2_PROXY_LIST>{
public:
    List(const ldd::protocol::ListMessage &req) : req_(req) {}
    virtual ~List() {}

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
    ldd::protocol::ListMessage req_;
    ldd::protocol::GetOrListResponse res_;
};

