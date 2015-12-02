#ifndef __C2P_CHECK_H__
#define __C2P_CHECK_H__

#include "ldd/util/atomic.h"

#include "ldd/net/payload.h"
#include "ldd/net/incoming_msg.h"

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/client/response.h"

class Proxy;
class P2SCheck;

class C2PCheck: public ldd::net::TypedIncomingMsg<LDD_CLIENT_2_PROXY_CHECK>{
public:
    C2PCheck(Proxy *proxy);
    virtual ~C2PCheck();

    void SendResult();
    void SendRequest();
    void SetCode(ldd::net::Code code){
        code_ = code;
    }

protected:
    virtual void Init(const Payload& request, Action* next);
    virtual void Emit(Payload* response, Action* next);
    virtual void Done(Code* code);
    virtual void Cancel() {}

private:
    friend class P2SCheck;
    Proxy *proxy_;
    int namespace_id_;
    std::size_t hash_val_;
    int bucket_id_;
    int farm_id_;
    ldd::protocol::CheckMessage req_;
    ldd::protocol::CheckResponse res_;
    //ldd::util::Atomic<P2SCheck *> p2s_check_;
    //boost::shared_ptr<P2SCheck> p2s_check_;
    ldd::net::Payload request_;
    ldd::net::Payload response_;
    ldd::net::Code code_;
};

#endif /*__C2P_CHECK_H__*/

