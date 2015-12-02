// context.h (2013-08-10)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_SKEL_CONTEXT_H_
#define LDD_SKEL_CONTEXT_H_

#include <ldd/net/event_loop.h>
#include <ldd/net/listener.h>
#include "ldd/skel/module.h"
#include "config.h"

namespace ldd {
namespace skel {

// ModuleCtx
class BaseModuleCtx : virtual public Module::ModuleCtx{
public:
    BaseModuleCtx(Config* config, ldd::net::EventLoop* event_loop)
        : config_(config), event_loop_(event_loop) {}
    virtual const ptree& config() const { return config_->properties(); }
    virtual ldd::net::EventLoop* event_loop() const { return event_loop_; }
    virtual const net::Address& addr() const { return config_->addr(); }
private:
    Config* config_;
    ldd::net::EventLoop* event_loop_;
};

class SimpleModuleCtx : public SimpleModule::ModuleCtx, public BaseModuleCtx {
public:
    SimpleModuleCtx(Config* config, ldd::net::Listener* listener, int port)
        : BaseModuleCtx(config, listener->event_loop()),
          listener_(listener), port_(port) {}
    virtual net::Listener* listener() const { return listener_; }
    virtual int port() const { return port_; }
private:
    ldd::net::Listener* listener_;
    int port_;
};

class WorkerModuleCtx : public WorkerModule::ModuleCtx, public BaseModuleCtx {
public:
    WorkerModuleCtx(Config* config, ldd::net::EventLoop* event_loop, int port)
        : BaseModuleCtx(config, event_loop), port_(port) {}
    virtual int port() const { return port_; }
private:
    int port_;
};

class SingleMasterModuleCtx : public SingleMasterModule::ModuleCtx,
                              public BaseModuleCtx {
public:
    SingleMasterModuleCtx(Config* config, ldd::net::EventLoop* event_loop,
            const std::set<int>& ports)
        : BaseModuleCtx(config, event_loop), ports_(ports) {}
    virtual const std::set<int>& ports() const { return ports_; }
private:
    std::set<int> ports_;
};


// WorkerCtx
class BaseWorkerCtx : virtual public Module::WorkerCtx {
public:
    BaseWorkerCtx(ldd::net::Listener* listener)
        : listener_(listener) {}
    virtual ldd::net::EventLoop* event_loop() const {
        return listener_->event_loop();
    }
    virtual net::Listener* listener() const {
        return listener_;
    }
private:
    ldd::net::Listener* listener_;
};

class WorkerWorkerCtx : public WorkerModule::WorkerCtx, public BaseWorkerCtx {
public:
    WorkerWorkerCtx(ldd::net::Listener* listener)
        : BaseWorkerCtx(listener) {}
};

class SingleMasterWorkerCtx : public SingleMasterModule::WorkerCtx,
                              public BaseWorkerCtx {
public:
    SingleMasterWorkerCtx(ldd::net::Listener* listener, int port)
        : BaseWorkerCtx(listener), port_(port) {}
    virtual int port() const { return port_; }
private:
    int port_;
};

} // namespace skel
} // namespace ldd

#endif // LDD_SKEL_CONTEXT_H_
