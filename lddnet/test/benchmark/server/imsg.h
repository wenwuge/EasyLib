// imsg.h (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#ifndef IMSG_H_
#define IMSG_H_

#include <ldd/net/incoming_msg.h>

class MyEchoMsg : public ldd::net::TypedIncomingMsg<2000> {
public:
    MyEchoMsg() {}
    virtual void Init(const Payload& request, Action* next);
    virtual void Emit(Payload* response, Action* next);
    virtual void Done(Code* code);
    virtual void Cancel();
private:
    Payload payload_;
};
#endif // IMSG_H_
