// p2s_get.h (2013-07-10)
// Yan Gaofeng (yangaofeng@360.cn)

#ifndef __P2S_LIST_H__
#define __P2S_LIST_H__

#include "ldd/net/outgoing_msg.h"
#include "ldd/protocol/server/command_type.h"
#include "ldd/protocol/server/store_proto.h"
#include "ldd/protocol/client/response.h"

class C2PList;

class P2SList : public ldd::net::TypedOutgoingMsg<LDD_PROXY_2_STORE_LIST> {
public:
    P2SList(C2PList *list, int index,
        int recv_timeout,
        int done_timeout) 
        : c2p_list_(list), index_(index),
        recv_timeout_(recv_timeout),
        done_timeout_(done_timeout) {

    }
    virtual ~P2SList() {}
    void CompleteRequest();

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout) ;
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout) ;
    void Done(const Result& result);

private:
    friend class C2PList;
    C2PList *c2p_list_;
    int index_;
    ldd::protocol::ListMessageS req_;

    int recv_timeout_;
    int done_timeout_;
};

#endif /*__P2S_LIST_H__*/

