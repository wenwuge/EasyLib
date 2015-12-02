// template.cc (2013-08-07)
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

class TemplateModule;

class MyEchoMsg : public ldd::net::TypedIncomingMsg<kMyEchoType> {
public:
    MyEchoMsg()
    {
    }
private:
    virtual void Init(const Payload& request, Action* next) {
        string x = request.body().ToString();
        LOG(INFO) << "Request recved: " << x;
        *next = kEmit;
    }
    virtual void Emit(Payload* response, Action* next) {
        LOG(INFO) << "Emit response";
        *response = payload_;
        *next = kDone;
        LOG(INFO) << "Emit ok";
    }
    virtual void Done(Code* code) {
        *code = 0;
        LOG(INFO) << "Done";
    }

    virtual void Cancel() {
        LOG(INFO) << "Cancel called";
    }

private:
    Payload payload_;
};

class TemplateModule : public ldd::skel::SingleMasterModule {
public:
    TemplateModule()
        : worker_(false),
          loop_(NULL),
          server_(NULL)
    {
    }

    ~TemplateModule() {
    }

    virtual bool InitModule(ModuleCtx* ctx) {
        LOG(INFO) << "Listened on addres: " << ctx->addr().ToString();
        // 获取配置
        try {
            count_ = ctx->config().get<size_t>("echo.count", 1 /*default*/);
        } catch (std::exception&e) {
            return false;
        }

        loop_ = ctx->event_loop();
        return true;
    }

    virtual bool InitWorker(WorkerCtx* ctx) {
        loop_ = ctx->event_loop();
        worker_ = true;

        ldd::net::Server::Options options;
        options.pulse_interval = 1000;
        options.notify_connected =
            boost::bind(&TemplateModule::OnConnected, this, _1);
        options.notify_closed =
            boost::bind(&TemplateModule::OnClosed, this, _1);
        server_ = new ldd::net::Server(options);
        if (!server_->Start(ctx->listener())) {
            return false;
        }
        server_->RegisterIncomingMsg<MyEchoMsg>();
        return true;
    }

private:
    void OnConnected(const boost::shared_ptr<ldd::net::Channel>& c) {
    }
    void OnClosed(const boost::shared_ptr<ldd::net::Channel>& c) {
    }

private:
    bool worker_;
    ldd::net::EventLoop* loop_;
    ldd::net::Server* server_;

    size_t count_;
};

LDD_SKEL_SINGLE_MASTER_MODULE(TemplateModule);

