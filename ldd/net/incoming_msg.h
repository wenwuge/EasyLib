#ifndef LDD_NET_INCOMING_MSG_H_
#define LDD_NET_INCOMING_MSG_H_

#include <map>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include "ldd/net/payload.h"
#include "ldd/net/message.h"

namespace ldd {
namespace net {

class Channel;

class IncomingMsg : public boost::enable_shared_from_this<IncomingMsg>,
                    private boost::noncopyable {
public:
    class Impl;
    typedef ldd::net::Payload Payload;
    typedef ldd::net::MessageId Id;
    typedef ldd::net::MessageType Type;
    typedef ldd::net::Code Code;

    enum Action { kWait, kEmit, kDone };

    virtual ~IncomingMsg();
    virtual Type type() const = 0;
    
    Id id() const;
    bool IsCanceled() const;
    const boost::shared_ptr<Channel>& channel() const;

    // Notify the message do Emit/Done action.
    // Requires: previous Init()/Step() returns kWait.
    void Notify(bool done = false);
protected:
    IncomingMsg();
    // Called by the library to parse the request payload.
    virtual void Init(const Payload& request, Action* next) = 0;
    // Called by the library to prepare the response payload.
    virtual void Emit(Payload* response, Action* next) = 0;
    // Called by the library to clean up this message.
    virtual void Done(Code* code) = 0;
    // Called by the library to notify the message to be canceled.
    virtual void Cancel() = 0;
private:
    void DoNotify(bool);
    Impl* impl_;
    friend class Channel;
    friend class IncomingRequest;
    friend class IncomingCancel;
};

template <MessageType N>
class TypedIncomingMsg : public IncomingMsg {
public:
    static const Type kType = N;
    Type type() const { return kType; }
protected:
    virtual void Init(const Payload& request, Action* next) = 0;
    virtual void Emit(Payload* response, Action* next) = 0;
    virtual void Done(Code* code) = 0;
    virtual void Cancel() {}
};


class DynamicIncomingMsg : public IncomingMsg {
public:
    ~DynamicIncomingMsg(); 
    void set_type(Type type);
    Type type() const { return type_; }
protected:
    DynamicIncomingMsg() : type_(0) {}
    virtual void Init(const Payload& request, Action* next) = 0;
    virtual void Emit(Payload* response, Action* next) = 0;
    virtual void Done(Code* code) = 0;
    virtual void Cancel() {}
private:
    Type type_;

#if 0
private:
    friend class Stat;

    struct NextFree {
        NextFree() 
            : next_id_(new util::Atomic<uint64_t>()), 
              free_id_(new util::Atomic<uint64_t>())
        { *next_id_ = 0; *free_id_ = 0; }
        ~NextFree() {}

        boost::shared_ptr<util::Atomic<uint64_t> > next_id_;
        boost::shared_ptr<util::Atomic<uint64_t> > free_id_;
    };

    static std::map<Type,  NextFree> stat_;

    typedef std::map<Type, NextFree>::iterator STAT_IT;

    static void Inc(Type type);
    static void Dec(Type type);
#endif
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_INCOMING_MSG_H_

