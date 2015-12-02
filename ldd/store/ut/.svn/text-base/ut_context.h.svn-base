#ifndef __LDD__STORAGE_UT_CONTEXT_H_
#define __LDD__STORAGE_UT_CONTEXT_H_

#include <stdint.h>
#include "../server/command_type.h"
#include "net/outgoing_msg.h"

namespace ldd{
    namespace protocol{
        class PrepareMessageS;
        class MutateMessageS;
        class Message;
        class AtomicIncrMessageS;
        class AtomicAppendMessageS;
    }
}


namespace ldd{
namespace storage{

using namespace ldd::net;
using ldd::protocol::PrepareMessageS;
using ldd::protocol::Message;
using ldd::protocol::MutateMessageS;
using ldd::protocol::AtomicIncrMessageS;
using ldd::protocol::AtomicAppendMessageS;

class ConfigManager;
class ModifyBaseTask;

class ReadContext : 
    public TypedOutgoingMsg<LDD_PROXY_2_STORE_GET>{
public:
    ReadContext(Message* gms) :
      gms_(gms){

      };
      virtual ~ReadContext();

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
    Message* gms_;
    net::Buffer buffer_;
};

class CheckContext : 
    public TypedOutgoingMsg<LDD_PROXY_2_STORE_CHECK>{
public:
    CheckContext(Message* gms) :
      gms_(gms){

      };
      virtual ~CheckContext();

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
    Message* gms_;
    net::Buffer buffer_;
};

class MutateContext : 
    public TypedOutgoingMsg<LDD_PROXY_2_STORE_MUTATE>{
public:
    MutateContext(Message* gms) :
      gms_(gms){

      };
      virtual ~MutateContext();

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
    Message* gms_;
    net::Buffer buffer_;
};

class ListContext : 
    public TypedOutgoingMsg<LDD_PROXY_2_STORE_LIST>{
public:
    ListContext(Message* gms) :
      gms_(gms){

      };
      virtual ~ListContext();

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
    Message* gms_;
    net::Buffer buffer_;
};

class AtomicIncrContext : 
    public TypedOutgoingMsg<LDD_PROXY_2_STORE_ATOMIC_INCR>{
public:
    AtomicIncrContext(Message* gms) : 
      gms_(gms){

      };
      virtual ~AtomicIncrContext();

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
    Message* gms_;
    net::Buffer buffer_;
};

class AtomicAppendContext : 
    public TypedOutgoingMsg<LDD_PROXY_2_STORE_ATOMIC_APPEND>{
public:
    AtomicAppendContext(Message* gms) :
      gms_(gms){

      };
      virtual ~AtomicAppendContext();

protected:
    bool Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);

private:
    Message* gms_;
    net::Buffer buffer_;
};


}//namespace storage
}//namespace ldd





#endif //__LDD__STORAGE_UT_CONTEXT_H_
