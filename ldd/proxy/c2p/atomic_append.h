#ifndef __C2P_ATOMIC_APPEND_H__
#define __C2P_ATOMIC_APPEND_H__

#include "ldd/util/atomic.h"

#include "ldd/net/payload.h"
#include "ldd/net/incoming_msg.h"

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/client/response.h"

class Proxy;
class P2SAtomicAppend;

class C2PAtomicAppend: 
    public ldd::net::TypedIncomingMsg<LDD_CLIENT_2_PROXY_ATOMIC_APPEND>{
public:
    C2PAtomicAppend(Proxy *proxy);
    virtual ~C2PAtomicAppend();


    void SendRequest();
    void SendResult();
    void SetCode(ldd::net::Code code){
        code_ = code;
    }

protected:
    virtual void Init(const Payload& request, Action* next);
    virtual void Emit(Payload* response, Action* next);
    virtual void Done(Code* code);
    virtual void Cancel() {}

private:
    friend class P2SAtomicAppend;
    Proxy *proxy_;
    int namespace_id_;
    std::size_t hash_val_;;
    int bucket_id_;
    int farm_id_;
    ldd::protocol::AtomicAppendMessage req_;
    ldd::protocol::CasResponse res_;
    //ldd::util::Atomic<P2SAtomicAppend *> p2s_atomic_append_;
    //boost::shared_ptr<P2SAtomicAppend> p2s_atomic_append_;
    ldd::net::Payload request_;
    ldd::net::Payload response_;
    ldd::net::Code code_;
};

#endif /*__C2P_APPEND_H__*/

