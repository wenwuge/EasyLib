// imsg.cc (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#include <glog/logging.h>
#include <boost/make_shared.hpp>
#include <ldd/net/channel.h>
#include "imsg.h"
#include "omsg.h"

void MyEchoMsg::Init(const Payload& request, Action* next) {
    payload_ = request;
    LOG(INFO) << "MyEchoMsg::Init(): " << payload_.body().ToString();
    LOG(INFO) << "           Extra:  " << payload_.extras().at(1).ToString();
    if (n_ < count_) {
        *next = kEmit;
    } else {
        *next = kDone;
    }
}

void MyEchoMsg::Emit(Payload* response, Action* next) {
    LOG(INFO) << "MyEchoMsg::Emit()";
    CHECK_LT(n_, count_);
    *response = payload_;
    if (++n_ < count_) {
        *next = kEmit;
    } else {
        *next = kDone;
    }
}

void MyEchoMsg::Done(Code* code) {
    LOG(INFO) << "MyEchoMsg::Done()";
    *code = count_;
    boost::shared_ptr<MyEmitMsg> msg = 
        boost::make_shared<MyEmitMsg>(payload_.body().ToString(),
                payload_.extras().at(1).ToString());
    channel()->Post(msg);
}

void MyEchoMsg::Cancel() {
    LOG(INFO) << "MyEchoMsg::Cancel()";
}
