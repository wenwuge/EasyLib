// example.cc (2013-08-07)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/bind.hpp>
#include <ldd/skel/module.h>
#include <ldd/net/incoming_msg.h>
#include <ldd/net/event_loop.h>
#include <ldd/net/event.h>
#include <ldd/net/server.h>
#include <ldd/util/time.h>
#include <glog/logging.h>

using namespace std;
static const int kMyEchoType = 2100;

class ExampleModule;

class MyEchoMsg : public ldd::net::TypedIncomingMsg<kMyEchoType> {
public:
    MyEchoMsg(size_t count)
        : count_(count), n_(0)
    {
    }
private:
    virtual void Init(const Payload& request, Action* next) {
        string x = request.body().ToString();
        LOG(INFO) << "Request recved: " << x;
        if (count_ > 0) {
            payload_ = request;
            *next = kEmit;
        } else {
            *next = kDone;
        }
    }
    virtual void Emit(Payload* response, Action* next) {
        LOG(INFO) << "Emit response";
        response->SetBody(payload_.body());
        if (++n_ == count_) {
            *next = kDone;
            LOG(INFO) << "Emit ok";
        } else {
            *next = kEmit;
        }
    }
    virtual void Done(Code* code) {
        *code = 0;
        LOG(INFO) << "Done";
    }

    virtual void Cancel() {
        LOG(INFO) << "Cancel called";
    }

private:
    size_t count_;
    size_t n_;
    Payload payload_;
};

class ExampleModule : public ldd::skel::SimpleModule {
public:
    ExampleModule()
    {
    }

    ~ExampleModule() {
    }

    virtual bool InitModule(ModuleCtx* ctx) {
        timeval tv = {};
        gettimeofday(&tv, NULL);
        int x = tv.tv_usec % 3;
        if (x == 0) {
            LOG(ERROR) << "failed";
            google::FlushLogFiles(0);
            return false;
        }
        if (x == 1) {
            LOG(ERROR) << "sleep";
            google::FlushLogFiles(0);
            sleep(6);
        }
        LOG(INFO) << "InitModule listen on address: "
            << ctx->addr().ToString();
        // 获取配置
        try {
            count_ = ctx->config().get<size_t>("echo.count", 1 /*default*/);
        } catch (std::exception&e) {
            LOG(ERROR) << "parse config failed";
            return false;
        }


        ldd::net::Server::Options options;
        options.pulse_interval = 1000;
        options.notify_connected =
            boost::bind(&ExampleModule::OnConnected, this, _1);
        options.notify_closed =
            boost::bind(&ExampleModule::OnClosed, this, _1);
        ldd::net::Server* server = new ldd::net::Server(options);
        if (!server->Start(ctx->listener())) {
            LOG(ERROR) << "Start server failed";
            return false;
        }
        server->RegisterIncomingMsg<MyEchoMsg>(count_);

        loop_ = ctx->event_loop();
        server_ = server;
        return true;
    }

private:
    void OnConnected(const boost::shared_ptr<ldd::net::Channel>& c) {
    }
    void OnClosed(const boost::shared_ptr<ldd::net::Channel>& c) {
    }

private:
    ldd::net::EventLoop* loop_;
    ldd::net::Server* server_;

    size_t count_;
};

LDD_SKEL_SIMPLE_MODULE(ExampleModule);

