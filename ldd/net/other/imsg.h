// imsg.h (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#ifndef IMSG_H_
#define IMSG_H_

#include <ldd/net/incoming_msg.h>

class MyEchoMsg : public ldd::net::TypedIncomingMsg<1333> {
public:
    MyEchoMsg(size_t count) : count_(count), n_(0) {}
    virtual void Init(const Payload& request, Action* next);
    virtual void Emit(Payload* response, Action* next);
    virtual void Done(Code* code);
    virtual void Cancel();
private:
    size_t count_;
    size_t n_;
    Payload payload_;
};
#endif // IMSG_H_
