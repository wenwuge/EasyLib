// imsg.cc (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#include <glog/logging.h>
#include <boost/make_shared.hpp>
#include <ldd/net/channel.h>
#include "imsg.h"
#include "omsg.h"
#include "my_lddnet.h"


static bool echo_ = false;

void MyEchoMsg::Init(const Payload& request, Action* next) {
    payload_ = request;

    LOG(INFO) << "MyEchoMsg::Init(): " << payload_.body().ToString();
    LOG(INFO) << "           Extra:  " << payload_.extras().at(1).ToString();


    if (REQUEST != payload_.body().ToString()
            || EXTRAS != payload_.extras().at(1).ToString()) {
        LOG(ERROR) << "unexpect body or extra";
        ldd_net_->set_ok(false);
    }

#if 0
    //for test bad case
    ldd_net_->set_ok(false);
#endif

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

    if (echo_) {
        LOG(INFO) << "echo complete";
        ldd_net_->event_loop()->Stop();
    }

    if (!echo_) {
        boost::shared_ptr<MyEmitMsg> msg = 
            boost::make_shared<MyEmitMsg>(ldd_net_, payload_.body().ToString(),
                    payload_.extras().at(1).ToString());
        channel()->Post(msg);

        echo_ = true;
    }
}

void MyEchoMsg::Cancel() {
    LOG(INFO) << "MyEchoMsg::Cancel()";
}

