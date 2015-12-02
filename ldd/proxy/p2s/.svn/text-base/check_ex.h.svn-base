// p2s_get.h (2013-07-10)
// Yan Gaofeng (yangaofeng@360.cn)

#ifndef __P2S_CHECK_H__
#define __P2S_CHECK_H__

#include "ldd/net/outgoing_msg.h"
#include "ldd/protocol/server/command_type.h"
#include "ldd/protocol/server/store_proto.h"
#include "ldd/protocol/client/response.h"

class C2PCheck;

class P2SCheck : 
        public ldd::net::TypedOutgoingMsg<LDD_PROXY_2_STORE_CHECK> {
public:
    P2SCheck(boost::shared_ptr<C2PCheck>check,
        int recv_timeout,
        int done_timeout) :
    c2p_check_(check),
        recv_timeout_(recv_timeout),
        done_timeout_(done_timeout) {

    }
    virtual ~P2SCheck() {}

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout) ;
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout) ;
    void Done(const Result& result);

private:
    friend class C2PCheck;
    boost::shared_ptr<C2PCheck> c2p_check_;
    ldd::protocol::CheckMessageS req_;

    int recv_timeout_;
    int done_timeout_;
};

#endif /*__P2S_CHECK_H__*/

