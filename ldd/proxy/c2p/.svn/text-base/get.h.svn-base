#ifndef __C2P_GET_H__
#define __C2P_GET_H__

#include "ldd/util/atomic.h"

#include "ldd/net/payload.h"
#include "ldd/net/incoming_msg.h"

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/client/response.h"

class Proxy;
class P2SGet;

class C2PGet: public ldd::net::TypedIncomingMsg<LDD_CLIENT_2_PROXY_GET> {
public:
    C2PGet(Proxy *proxy);
    virtual ~C2PGet();

    void SendRequest();
    void SendResult(const ldd::net::Payload &buffer);
    void SetCode(ldd::net::Code code){
        code_ = code;
    }

protected:
    virtual void Init(const Payload& request, Action* next);
    virtual void Emit(Payload* response, Action* next);
    virtual void Done(Code* code);
    virtual void Cancel() {}


private:
    void PrintResult();

    friend class P2SGet;
    Proxy *proxy_;
    std::size_t hash_val_;
    int namespace_id_;
    int bucket_id_;
    int farm_id_;
    ldd::protocol::GetMessage req_;
    ldd::protocol::GetOrListResponse res_;
    //ldd::util::Atomic<P2SGet *> p2s_get_;
    //boost::shared_ptr<P2SGet>p2s_get_;
    ldd::net::Payload request_;
    ldd::net::Payload response_;
    ldd::net::Code code_;
};

#endif /*__C2P_GET_H__*/

