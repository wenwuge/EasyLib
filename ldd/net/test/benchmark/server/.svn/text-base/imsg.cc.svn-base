// imsg.cc (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#include <glog/logging.h>
#include <boost/make_shared.hpp>
#include <ldd/net/channel.h>
#include "imsg.h"

void MyEchoMsg::Init(const Payload& request, Action* next) {
    payload_ = request;
    //LOG(INFO) << "MyEchoMsg::Init(): " << payload_.body().ToString();
    //LOG(INFO) << "           Extra:  " << payload_.extras().at(1).ToString();
    *next = kEmit;
}

void MyEchoMsg::Emit(Payload* response, Action* next) {
    //LOG(INFO) << "MyEchoMsg::Emit()";
    *next = kDone;
}

void MyEchoMsg::Done(Code* code) {
    //LOG(INFO) << "MyEchoMsg::Done()";
    *code = 0;
}

void MyEchoMsg::Cancel() {
    //LOG(INFO) << "MyEchoMsg::Cancel()";
}
