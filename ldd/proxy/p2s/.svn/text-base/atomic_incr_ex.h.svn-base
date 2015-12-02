// p2s_get.h (2013-07-10)
// Yan Gaofeng (yangaofeng@360.cn)

#ifndef __P2S_ATOMIC_INCR_H__
#define __P2S_ATOMIC_INCR_H__

#include "ldd/net/outgoing_msg.h"
#include "ldd/protocol/server/command_type.h"
#include "ldd/protocol/server/store_proto.h"
#include "ldd/protocol/client/response.h"

class C2PAtomicIncr;

class P2SAtomicIncr : 
    public ldd::net::TypedOutgoingMsg <LDD_PROXY_2_STORE_ATOMIC_INCR>{
public:
    P2SAtomicIncr(boost::shared_ptr<C2PAtomicIncr>atomic_incr,
        int recv_timeout,
        int done_timeout) : 
    c2p_atomic_incr_(atomic_incr),
        recv_timeout_(recv_timeout),
        done_timeout_(done_timeout) {}
    virtual ~P2SAtomicIncr() {}


    //void SendResult(P2SAtomicIncr *atomic_incr);

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout) ;
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout) ;
    void Done(const Result& result);

private:
    friend class C2PAtomicIncr;
    boost::shared_ptr<C2PAtomicIncr> c2p_atomic_incr_;
    ldd::protocol::AtomicIncrMessageS req_;
    //ldd::protocol::CasResponse res_;

    int recv_timeout_;
    int done_timeout_;
};

#endif /*__P2S_ATOMIC_INCR_H__*/

