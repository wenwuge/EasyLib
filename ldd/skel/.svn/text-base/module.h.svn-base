// module.h (2013-08-05)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_SKEL_MODULE_H_
#define LDD_SKEL_MODULE_H_

#include <set>
#include <boost/property_tree/ptree.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <ldd/net/listener.h>

namespace ldd {
namespace skel {

class Module {
public:
    typedef boost::property_tree::ptree Config;
    enum Type {
        kTypeSimple,
        kTypeWorker,
        kTypeSingleMaster,
    };
    struct ModuleCtx {
        virtual ~ModuleCtx() {}
        virtual const Config& config() const = 0;
        virtual const net::Address& addr() const = 0;
        virtual net::EventLoop* event_loop() const = 0;
    };
    struct WorkerCtx {
        virtual ~WorkerCtx() {}
        virtual net::EventLoop* event_loop() const = 0;
        virtual net::Listener* listener() const = 0;
    };
    virtual ~Module() {}
    virtual Type type() = 0;
};

/**
 *      1 - Monitor
 *      N - Worker
 */ 
class SimpleModule : public Module {
public:
    Type type() { return kTypeSimple; }
    struct ModuleCtx : virtual public Module::ModuleCtx {
        virtual net::Listener* listener() const = 0;
        virtual int port() const = 0;
    };

    virtual bool InitModule(ModuleCtx* ctx) = 0;
};

class WorkerModule : public Module {
public:
    Type type() { return kTypeWorker; }
    struct ModuleCtx : virtual public Module::ModuleCtx {
        virtual int port() const = 0;
    };
    struct WorkerCtx : virtual public Module::WorkerCtx {
    };

    virtual bool InitModule(ModuleCtx* ctx) = 0;
    virtual bool InitWorker(WorkerCtx* ctx) = 0;
};

/**
 *  1 - Monitor
 *  1 - Master
 *  N - Worker
 *
 */
class SingleMasterModule : public Module {
public:
    Type type() { return kTypeSingleMaster; }
    struct ModuleCtx : virtual public Module::ModuleCtx {
        virtual const std::set<int>& ports() const = 0;
    };
    struct WorkerCtx : virtual public Module::WorkerCtx {
        virtual int port() const = 0;
    };

    virtual bool InitModule(ModuleCtx* ctx) = 0;
    virtual bool InitWorker(WorkerCtx* ctx) = 0;
};

} // namespace skel
} // namespace ldd

#define LDD_SKEL_SIMPLE_MODULE(M) \
    extern "C" ::ldd::skel::SimpleModule* LddSkel_NewSimpleModule() \
        {return new M;}
#define LDD_SKEL_WORKER_MODULE(M) \
    extern "C" ::ldd::skel::WorkerModule* LddSkel_NewWorkerModule() \
        {return new M;}
#define LDD_SKEL_SINGLE_MASTER_MODULE(M) \
    extern "C" ::ldd::skel::SingleMasterModule* LddSkel_NewSingleMasterModule()\
        {return new M;}

#endif // LDD_SKEL_MODULE_H_
