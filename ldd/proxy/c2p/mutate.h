#ifndef __C2P_MUTATE_H__
#define __C2P_MUTATE_H__

#include "ldd/util/atomic.h"

#include "ldd/net/payload.h"
#include "ldd/net/incoming_msg.h"

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/client/response.h"

class Proxy;
class P2SMutate;

class C2PMutate: public ldd::net::TypedIncomingMsg<LDD_CLIENT_2_PROXY_MUTATE>{
public:
    C2PMutate(Proxy *proxy);
    virtual ~C2PMutate();

    void SendResult();
    void SendRequest();
    void SetCode(ldd::net::Code code){
        code_ = code;
    }

protected:
    virtual void Init(const Payload& request, Action* next);
    virtual void Emit(Payload* response, Action* next);
    virtual void Done(Code* code);
    virtual void Cancel();

private:
    friend class P2SMutate;
    Proxy *proxy_;
    int namespace_id_;
    std::size_t hash_val_;
    int bucket_id_;
    int farm_id_;
    ldd::protocol::MutateMessage req_;
    ldd::protocol::MutateResponse res_;
    //ldd::util::Atomic<P2SMutate *> p2s_mutate_;
    //boost::shared_ptr<P2SMutate>p2s_mutate_;
    ldd::net::Payload request_;
    ldd::net::Payload response_;

    ldd::net::Code  code_;
};

#endif /*__C2P_MUTATE_H__*/

