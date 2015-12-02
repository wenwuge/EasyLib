// p2s_get.h (2013-07-10)
// Yan Gaofeng (yangaofeng@360.cn)

#ifndef __P2S_GET_H__
#define __P2S_GET_H__

#include "ldd/net/outgoing_msg.h"
#include "ldd/protocol/server/command_type.h"
#include "ldd/protocol/server/store_proto.h"
#include "ldd/protocol/client/response.h"

#include "ldd/proxy/c2p/get.h"

class P2SGet : public ldd::net::TypedOutgoingMsg<LDD_PROXY_2_STORE_GET> {
public:
    P2SGet(boost::shared_ptr<C2PGet> c2p_get,
        int recv_timeout,
        int done_timeout) : 
    c2p_get_(c2p_get),
        recv_timeout_(recv_timeout),
        done_timeout_(done_timeout) {

    }
    virtual ~P2SGet() {}
    void PrintResult();

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout) ;
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout) ;
    void Done(const Result& result);

private:
    friend class C2PGet;
    boost::shared_ptr<C2PGet> c2p_get_;
    ldd::protocol::GetMessageS req_;

    int recv_timeout_;
    int done_timeout_;
};

#endif /*__P2S_GET_H__*/

