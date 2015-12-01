// omsg.cc (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#include <glog/logging.h>
#include "omsg.h"

bool MyEmitMsg::Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout) {
    LOG(INFO) << "OutMsg Send: " << str_;
    ldd::net::Buffer buf(str_);
    request->SetBody(buf);
    LOG(INFO) << "XXX " << str2_;
    CHECK(request->SetExtra(1, ldd::net::Buffer(str2_)));
    return true;
}

bool MyEmitMsg::Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout) {
    std::string s(response.body().data(), response.body().size());
    LOG(INFO) << "OutMsg Recv: " << response.body().ToString();
    LOG(INFO) << "       Extra:" << response.extras().at(1).ToString();
    return true;
}

void MyEmitMsg::Done(const Result& result) {
    LOG(INFO) << "OutMsg Done: " << result.ToString() << " " << result.code();
}
