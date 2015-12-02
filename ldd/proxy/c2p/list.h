#ifndef __C2P_LIST_H__
#define __C2P_LIST_H__

#include <list>

#include "ldd/util/atomic.h"
#include "ldd/util/mutex.h"

#include "ldd/net/payload.h"
#include "ldd/net/incoming_msg.h"

#include "ldd/protocol/client/command_type.h"
#include "ldd/protocol/client/proto.h"
#include "ldd/protocol/client/response.h"

class Proxy;
class P2SList;

class C2PList: public ldd::net::TypedIncomingMsg<LDD_CLIENT_2_PROXY_LIST>{
public:
    C2PList(Proxy *proxy);
    virtual ~C2PList();

    void SendRequest();
    void SendResult(ldd::protocol::GetOrListResponse *res, 
                    ldd::net::Payload result, int index);
    void SetCode(ldd::net::Code code){
        code_ = code;
    }
    void Complete();

protected:
    virtual void Init(const Payload& request, Action* next);
    virtual void Emit(Payload* response, Action* next);
    virtual void Done(Code* code);
    virtual void Cancel();

private:
    friend class P2SList;
    Proxy *proxy_;
    std::size_t hash_val_;
    int namespace_id_;
    int bucket_id_;
    ldd::protocol::ListMessage req_;

    struct P2SListInfo {
        P2SListInfo() : complete(false), has_new_result(false) {

        }
        bool complete;
        bool has_new_result;
        std::list<ldd::protocol::GetOrListResponse *> response;
        std::vector<ldd::net::Payload> response_buff;
        //P2SList * p2s_list;  // replace by next sentence
        boost::shared_ptr<P2SList> p2s_list;
    };

    int cur_index_;
    std::vector<P2SListInfo> p2s_list_info_;
    uint32_t send_count_;
    ldd::util::Mutex mutex_;

    ldd::net::Payload response_;
    ldd::net::Code code_;
};

#endif /*__C2P_LIST_H__*/

