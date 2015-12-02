// p2s_get.h (2013-07-10)
// Yan Gaofeng (yangaofeng@360.cn)

#ifndef __P2S_ATOMIC_APPEND_H__
#define __P2S_ATOMIC_APPEND_H__

#include "ldd/net/outgoing_msg.h"
#include "ldd/protocol/server/command_type.h"
#include "ldd/protocol/server/store_proto.h"
#include "ldd/protocol/client/response.h"

class C2PAtomicAppend;

class P2SAtomicAppend : 
        public ldd::net::TypedOutgoingMsg<LDD_PROXY_2_STORE_ATOMIC_APPEND>{
public:
    P2SAtomicAppend(boost::shared_ptr<C2PAtomicAppend> atomic_append,
        int recv_timeout,
        int done_timeout) 
        : c2p_atomic_append_(atomic_append),
        recv_timeout_(recv_timeout),
        done_timeout_(done_timeout) {

    }
    virtual ~P2SAtomicAppend() {}

protected:
    bool Init(Payload* request,
                    ldd::util::TimeDiff* recv_timeout,
                    ldd::util::TimeDiff* done_timeout) ;
    bool Recv(const Payload& response,
                    ldd::util::TimeDiff* recv_timeout) ;
    void Done(const Result& result);

private:
    friend class C2PAtomicAppend;
    boost::shared_ptr<C2PAtomicAppend> c2p_atomic_append_;
    ldd::protocol::AtomicAppendMessageS req_;
    //ldd::protocol::CasResponse res_;
    int recv_timeout_;
    int done_timeout_;
};

#endif /*__P2S_ATOMIC_APPEND_H__*/

