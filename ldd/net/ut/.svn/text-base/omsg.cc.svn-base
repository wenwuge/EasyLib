// omsg.cc (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#include <glog/logging.h>
#include "omsg.h"
#include "my_lddnet.h"

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
    LOG(INFO) << "OutMsg Recv: " << response.body().ToString();
    LOG(INFO) << "       Extra:" << response.extras().at(1).ToString();

    if (response.body().ToString() != REQUEST
            || response.extras().at(1).ToString() != EXTRAS) {
        LOG(ERROR) << "unexpected response or extras";
        ldd_net_->set_ok(false);
    }

    return true;
}

void MyEmitMsg::Done(const Result& result) {
    LOG(INFO) << "OutMsg Done: " << result.ToString() << " " << result.code();
}
