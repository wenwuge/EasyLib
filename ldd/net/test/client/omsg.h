// omsg.h (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#ifndef OMSG_H_
#define OMSG_H_

#include <ldd/net/outgoing_msg.h>

class MyEmitMsg : public ldd::net::TypedOutgoingMsg<1333> {
public:
    MyEmitMsg(const std::string& str, const std::string& str2)
        : str_(str), str2_(str2) {}

    bool Init(Payload* request,
            ldd::util::TimeDiff* recv_timeout,
            ldd::util::TimeDiff* done_timeout);
    bool Recv(const Payload& response,
            ldd::util::TimeDiff* recv_timeout);
    void Done(const Result& result);
private:
    std::string str_;
    std::string str2_;
};

#endif // OMSG_H_
