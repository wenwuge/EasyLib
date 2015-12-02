// child.h (2013-08-10)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_SKEL_CHILD_H_
#define LDD_SKEL_CHILD_H_

#include <ldd/net/event_loop.h>
#include <ldd/net/event.h>
#include <ldd/net/listener.h>
#include "ldd/skel/module.h"

namespace ldd {
namespace skel {

typedef boost::ptr_map<int, net::Listener> ListenerMap;

class Config;

class Child {
public:
    Child(Config* config, Module::Type type);
    virtual ~Child();

    bool Init();
    void Run();
protected:
    Module* module() const { return module_; }
    Config* config() const { return config_; }
    Module::Type type() const { return type_; }
    const std::string& title() const { return title_; }
    void set_title(const std::string& title) { title_ = title; }

    virtual bool DoInit() = 0;
    virtual void DoRun() = 0;
    virtual void DoCleanUp() {}
    virtual void HandleChild();
private:
    bool OpenModule();
    void HandleSignal(int sig);
    void HandleStop();
    
    void* dlmodule_;
    Module* module_;
    Config* config_;
    Module::Type type_;
    
    std::string title_;
protected:
    net::EventLoop* loop_;
    net::SignalEvent* sigev_;
};

class SinglePortChild : public Child {
public:
    SinglePortChild(Config* config, Module::Type type,
            int port, ldd::net::Listener* listener);
protected:
    int port() const { return port_; }
    ldd::net::Listener* listener() const { return listener_; }
private:
    int port_;
    ldd::net::Listener* listener_;
};

class SimpleChild : public SinglePortChild {
public:
    SimpleChild(Config* config, int port, ldd::net::Listener* listener);
    ~SimpleChild();
private:
    virtual bool DoInit();
    virtual void DoRun();

    SimpleModule* module() const {
        return dynamic_cast<SimpleModule*>(Child::module());
    }
};

class WorkerChild : public SinglePortChild {
public:
    WorkerChild(Config* config, int port,
            ldd::net::Listener* listener);
    ~WorkerChild();
private:
    virtual bool DoInit();
    virtual void DoRun();
    virtual void DoCleanUp();
    virtual void HandleChild();
    WorkerModule* module() const {
        return dynamic_cast<WorkerModule*>(Child::module());
    }
    void RunWorker();
    void RebootWorker();
private:
    net::EventLoop* mloop_;
    net::EventLoop* wloop_;
    pid_t worker_;
    net::TimerEvent* timer_;
};

class SingleMasterChild : public Child {
public:
    SingleMasterChild(Config* config, net::EventLoop* event_loop,
            boost::ptr_map<int, ldd::net::Listener>& listeners);
    ~SingleMasterChild();
    virtual bool DoInit();
    virtual void DoRun();
    virtual void DoCleanUp();
    virtual void HandleChild();
private:
    SingleMasterModule* module() const {
        return dynamic_cast<SingleMasterModule*>(Child::module());
    }
    void RunWorkers();
    void RunWorker(int port);
    void RebootWorker(int port);
private:
    boost::ptr_map<int, ldd::net::Listener>& listeners_;
    net::EventLoop* mloop_;
    net::EventLoop* wloop_;
    std::map<pid_t, int> workers_;
    std::map<int, net::TimerEvent*> timers_;
};

} // namespace skel
} // namespace ldd

#endif // LDD_SKEL_CHILD_H_
