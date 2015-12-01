// omsg.cc (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/make_shared.hpp>
#include <glog/logging.h>
#include <ldd/net/channel.h>
#include "omsg.h"
DECLARE_string(request);
DECLARE_string(extra);
DECLARE_int32(connections);
DECLARE_int32(concurrency);

extern struct Info g_info;

MyEmitMsg::MyEmitMsg(const std::string& str, const std::string& str2)
    : str_(str), str2_(str2), step_(0), ts_(ldd::util::Timestamp::Now())
{
    DLOG(INFO) << "MyEmitMsg ctor called";
}

MyEmitMsg::~MyEmitMsg()
{
    DLOG(INFO) << "MyEmitMsg dtor called";
}

bool MyEmitMsg::Init(Payload* request,
        ldd::util::TimeDiff* recv_timeout,
        ldd::util::TimeDiff* done_timeout) {
    step_ = 1;
    //LOG(INFO) << "OutMsg Send: " << str_;
    ldd::net::Buffer buf(str_);
    request->SetBody(buf);
    //LOG(INFO) << "XXX " << str2_;
    CHECK(request->SetExtra(1, ldd::net::Buffer(str2_)));
    return true;
}

bool MyEmitMsg::Recv(const Payload& response,
        ldd::util::TimeDiff* recv_timeout) {
    step_ = 2;
    std::string s(response.body().data(), response.body().size());
    //LOG(INFO) << "OutMsg Recv: " << response.body().ToString();
    //LOG(INFO) << "       Extra:" << response.extras().at(1).ToString();
    return true;
}

void MyEmitMsg::Done(const Result& result) {
    if (!result.IsOk()) {
        LOG(ERROR) << "OutMsg " << result.ToString()
            << " id=" << id()
            << " step=" << step_
            << " time=" << (ldd::util::Timestamp::Now() - ts_).ToMilliseconds();
        g_info.failed++;
    } else {
        g_info.success++;
    }
    boost::shared_ptr<MyEmitMsg> msg = 
        boost::make_shared<MyEmitMsg>(FLAGS_request, FLAGS_extra);
    channel()->Post(msg, ldd::util::TimeDiff::Milliseconds(5), true);
    g_info.requests++;
}
