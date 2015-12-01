// imsg.h (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#ifndef IMSG_H_
#define IMSG_H_

#include <ldd/net/incoming_msg.h>

class MyLddNet;

class MyEchoMsg : public ldd::net::TypedIncomingMsg<1333> {
public:
    MyEchoMsg(MyLddNet *ldd_net) : count_(1), n_(0), ldd_net_(ldd_net) {}
    virtual void Init(const Payload& request, Action* next);
    virtual void Emit(Payload* response, Action* next);
    virtual void Done(Code* code);
    virtual void Cancel();
private:
    size_t count_;
    size_t n_;
    Payload payload_;

    MyLddNet *ldd_net_;

};
#endif // IMSG_H_
