#ifndef __C2P_ATOMIC_INCR_H__
#define __C2P_ATOMIC_INCR_H__

#include "ldd/util/atomic.h"

#include "ldd/net/payload.h"
#include "ldd/net/incoming_msg.h"

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/client/response.h"

class Proxy;
class P2SAtomicIncr;

class C2PAtomicIncr: 
    public ldd::net::TypedIncomingMsg<LDD_CLIENT_2_PROXY_ATOMIC_INCR>{
public:
    C2PAtomicIncr(Proxy *proxy);
    virtual ~C2PAtomicIncr();

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
    friend class P2SAtomicIncr;
    Proxy *proxy_;
    int namespace_id_;
    std::size_t hash_val_;
    int bucket_id_;
    int farm_id_;
    ldd::protocol::AtomicIncrMessage req_;
    ldd::protocol::CasResponse res_;
    //ldd::util::Atomic<P2SAtomicIncr *> p2s_atomic_incr_;
    //boost::shared_ptr<P2SAtomicIncr> p2s_atomic_incr_;
    ldd::net::Payload request_;
    ldd::net::Payload response_;
    ldd::net::Code code_;
};

#endif /*__C2P_ATOMIC_INCR_H__*/

