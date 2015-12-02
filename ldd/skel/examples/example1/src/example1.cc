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
#include <zookeeper/zookeeper.h>

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

class ExampleModule : public ldd::skel::SingleMasterModule {
public:
    ExampleModule()
        : zh_(NULL),
          zkev_(NULL),
          loop_(NULL),
          in_worker_(false)
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
        LOG(INFO) << "In InitModule Listened on addres: " 
            << ctx->addr().ToString();
        // 获取配置
        try {
            count_ = ctx->config().get<size_t>("echo.count", 1 /*default*/);
            zkhost_ = ctx->config().get("zookeeper.host", "localhost:2181");
        } catch (std::exception&e) {
            return false;
        }

        // 连接zookeeper
        // ...
        zh_ = zookeeper_init(zkhost_.c_str(), Watcher, 1000, NULL, this, 0);

        // 注册zookeeper fd到 ctx.loop() 上
        // ...
        zkev_ = new ldd::net::FdEvent(ctx->event_loop());
        DriveZk();
        loop_ = ctx->event_loop();
        return true;
    }

    virtual bool InitWorker(WorkerCtx* ctx) {
        loop_ = ctx->event_loop();
        in_worker_ = true;

        ldd::net::Server::Options options;
        options.pulse_interval = 1000;
        options.notify_connected =
            boost::bind(&ExampleModule::OnConnected, this, _1);
        options.notify_closed =
            boost::bind(&ExampleModule::OnClosed, this, _1);
        ldd::net::Server* server = new ldd::net::Server(options);
        if (!server->Start(ctx->listener())) {
            return false;
        }
        server_ = server;
        server->RegisterIncomingMsg<MyEchoMsg>(count_);
        return true;
    }

private:
    void OnConnected(const boost::shared_ptr<ldd::net::Channel>& c) {
    }
    void OnClosed(const boost::shared_ptr<ldd::net::Channel>& c) {
    }

    void DriveZk() {
        struct timeval tv = {};
        int zkfd = 0;
        int interest = 0;
        int zkevents = 0;

        zookeeper_interest(zh_, &zkfd, &interest, &tv);
        if (interest & ZOOKEEPER_READ) {
            zkevents |= ldd::net::FdEvent::kReadable;
        }
        if (interest & ZOOKEEPER_WRITE) {
            zkevents |= ldd::net::FdEvent::kWritable;
        }
        zkev_->AsyncWait(zkfd, zkevents, 
                boost::bind(&ExampleModule::HandleZk, this, _1),
                ldd::util::TimeDiff(tv));
    }

    void ReInitZk() {
        // 重新连接zookeeper, 初始化zookeeper的节点状态。
        // ...
        
        // 或者直接退出进程，让 lddskel 重启自己
        loop_->Stop();
    }

    void HandleZk(int events) {
        int interest = 0;
        if (events & ldd::net::FdEvent::kReadable) {
            interest |= ZOOKEEPER_READ;
        }
        if (events & ldd::net::FdEvent::kWritable) {
            interest |= ZOOKEEPER_WRITE;
        }
        zookeeper_process(zh_, interest);

        if (is_unrecoverable(zh_)) {
            ReInitZk();
        } else {
            DriveZk();
        }
    }

    void WatchZk(zhandle_t* zh, int type, int state, const char* path) {
    }

    static void Watcher(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx) {
        ExampleModule* x = (ExampleModule*)watcherCtx;
        x->WatchZk(zh, type, state, path);
    }
private:
    zhandle_t* zh_;
    ldd::net::FdEvent* zkev_;
    ldd::net::EventLoop* loop_;
    ldd::net::Server* server_;
    bool in_worker_;

    string zkhost_;
    size_t count_;
};

LDD_SKEL_SINGLE_MASTER_MODULE(ExampleModule);

