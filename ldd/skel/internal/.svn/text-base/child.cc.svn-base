// child.cc (2013-08-10)
// Li Xinjie (xjason.li@gmail.com)

#include <sys/types.h>
#include <sys/wait.h>
#include <dlfcn.h>
#include <signal.h>
#include <sstream>
#include <glog/logging.h>
#include <boost/foreach.hpp>
#include <ldd/util/time.h>
#include <ldd/util/thread.h>
#include "child.h"
#include "option.h"
#include "proctitle.h"
#include "config.h"
#include "context.h"
#include "log_flusher.h"

namespace ldd {
namespace skel {

using namespace std;
using namespace boost;

const int kMaxWaitingWorker = 8* 1000LL;

const char* kModuleLoaderNames[] = {
    "LddSkel_NewSimpleModule",
    "LddSkel_NewWorkerModule",
    "LddSkel_NewSingleMasterModule"
};

typedef Module* (*Loader)();

Child::Child(Config* config, Module::Type type)
    : dlmodule_(NULL),
      module_(NULL),
      config_(config),
      type_(type),
      loop_(NULL),
      sigev_(NULL)
{
    CHECK_NOTNULL(config);
}

Child::~Child() {
    try {
        delete module_;
    } catch (const std::exception& e) {
        LOG(ERROR) << "Delete module with exception: " << e.what();
    } catch (...) {
        LOG(ERROR) << "Delete module with unknown error";
    }

    delete sigev_;
    delete loop_;
    delete config_;

    if (dlmodule_) {
        dlclose(dlmodule_);
    }
}

bool Child::Init() {
    stringstream title;
    title << "lddskel(" << Option::instance().name() << "): " << title_;
    ProcTitle::Set(title.str());

    sigev_ = new net::SignalEvent(loop_);
    sigev_->Add(SIGTERM);
    sigev_->Add(SIGQUIT);
    sigev_->Add(SIGINT);
    sigev_->Add(SIGHUP);
    if (type_ == Module::kTypeWorker || type_ == Module::kTypeSingleMaster) {
        sigev_->Add(SIGCHLD);
    }
    sigev_->AsyncWait(
            boost::bind(&Child::HandleSignal, this, _1));
    if (!OpenModule()) {
        return false;
    }
    if (!DoInit()) {
        return false;
    }
    return true;
}

void Child::HandleSignal(int sig) {
    switch (sig) {
    case SIGINT:
    case SIGTERM:
    case SIGQUIT:
        LOG(INFO) << "SIGTERM received";
        HandleStop();
        break;
    case SIGCHLD:
        LOG(INFO) << "SIGCHLD recevied";
        HandleChild();
        break;
    case SIGHUP:
        LOG(INFO) << "SIGHUP recevied";
        break;
    }
    sigev_->AsyncWait(
            boost::bind(&Child::HandleSignal, this, _1));
}

void Child::HandleStop() {
    loop_->Stop();
}

void Child::HandleChild() {
    CHECK(0) << "Shoud not be called";
}

bool Child::OpenModule() {
    void* ptr = dlopen(config_->file().c_str(), RTLD_LAZY);
    if (!ptr) {
        LOG(ERROR) << "dlopen(" << config_->file() << ", ...): " << dlerror();
        return false;
    }
    Module* module = NULL;
    const char* loader_name = kModuleLoaderNames[type_];
    Loader loader = (Loader)dlsym(ptr, loader_name);
    if (!loader) {
        LOG(ERROR) << "dlsym(" << loader_name << "): " << dlerror();
        dlclose(ptr);
        return false;
    }

    try {
        module = loader();
    } catch (const std::exception& e) {
        LOG(ERROR) << "Load module with exception: " << e.what();
        dlclose(ptr);
        return false;
    } catch (...) {
        LOG(ERROR) << "Load module failed with unknown error";
        dlclose(ptr);
        return false;
    }
    module_ = module;
    dlmodule_ = ptr;
    return true;
}

void Child::Run() {
    LogFlusher flusher(loop_, config()->log_flush());
    DoRun();
    DoCleanUp();
}

SinglePortChild::SinglePortChild(Config* config, Module::Type type,
        int port, ldd::net::Listener* listener)
    : Child(config, type),
      port_(port),
      listener_(listener)
{
}

SimpleChild::SimpleChild(Config* config, int port,
        ldd::net::Listener* listener)
    : SinglePortChild(config, Module::kTypeSimple, port, listener)
{
    stringstream ss;
    ss << "child process [" << port << "]";
    set_title(ss.str());

    loop_ = listener->event_loop();
}

SimpleChild::~SimpleChild() {
}

bool SimpleChild::DoInit() {
    if (!module()) {
        LOG(ERROR) << "Invalid module object type";
        return false;
    }
    try {
        SimpleModuleCtx ctx(config(), listener(), port());
        LOG(INFO) << "Calling InitModule()";
        if (!module()->InitModule(&ctx)) {
            LOG(ERROR) << "Module init failed";
            return false;
        }
    } catch (const std::exception& e) {
        LOG(ERROR) << "Module init with exception: " << e.what();
        return false;
    } catch (...) {
        LOG(ERROR) << "Module init failed with unknown error";
        return false;
    }
    return true;
}

void SimpleChild::DoRun() {
    loop_->Run();
}

WorkerChild::WorkerChild(Config* config, int port,
        ldd::net::Listener* listener)
    : SinglePortChild(config, Module::kTypeWorker, port, listener),
      mloop_(new net::EventLoop),
      wloop_(listener->event_loop()),
      worker_(0),
      timer_(NULL)
{
    stringstream ss;
    ss << "master process [" << port << "]";
    set_title(ss.str());

    loop_ = mloop_;
}

WorkerChild::~WorkerChild()
{
    loop_ = NULL;
    delete timer_;
    delete mloop_;
    delete wloop_;
}

bool WorkerChild::DoInit() {
    if (!module()) {
        LOG(ERROR) << "Invalid module object type";
        return false;
    }
    try {
        WorkerModuleCtx ctx(config(), loop_, port());
        LOG(INFO) << "Calling InitModule()";
        if (!module()->InitModule(&ctx)) {
            LOG(ERROR) << "Module init failed";
            return false;
        }
    } catch (const std::exception& e) {
        LOG(ERROR) << "Module init with exception: " << e.what();
        return false;
    } catch (...) {
        LOG(ERROR) << "Module init failed with unknown error";
        return false;
    }
    return true;
}

void WorkerChild::RunWorker() {
    CHECK_EQ(worker_, 0);

    google::FlushLogFiles(0);
    int fd[2];
    pipe(fd);
    char buf = 1;
    pid_t pid = fork();
    if (pid < 0) {
        PLOG(ERROR) << "fork worker failed";
        return;
    } else if (pid > 0) {
        // in child
        worker_ = pid;
        close(fd[0]);
        write(fd[1], &buf, 1);
        close(fd[1]);
        LOG(INFO) << "forked worker on " << pid;
        return;
    }

    // in worker
    close(fd[1]);
    read (fd[0], &buf, 1);
    close(fd[0]);

    google::ShutdownGoogleLogging();
    std::string logname = Option::instance().name() + "-worker";
    // run
    google::InitGoogleLogging(logname.c_str());
    LOG(INFO) << "worker started";

    // prepare and cleanup
    stringstream title;
    title << "lddskel(" << Option::instance().name() << "): worker process ["
            << port() << "]";
    ProcTitle::Set(title.str());

    mloop_->AfterFork();
    wloop_->AfterFork();
    delete sigev_;
    delete timer_;
    delete mloop_;
    timer_ = NULL;
    sigev_ = NULL;
    mloop_ = NULL;
    loop_ = wloop_;

    try {
        WorkerWorkerCtx ctx(listener());
        LOG(INFO) << "Calling InitWorker()";
        if (!module()->InitWorker(&ctx)) {
            LOG(ERROR) << "Worker init failed";
            goto finish;
        }
    } catch (const std::exception& e) {
        LOG(ERROR) << "Worker init with exception: " << e.what();
        goto finish;
    } catch (...) {
        LOG(ERROR) << "Worker init failed with unknown error";
        goto finish;
    }
    {
        LogFlusher flusher(loop_, config()->log_flush());
        loop_->Run();
    }
finish:
    delete this;
    google::FlushLogFiles(0);
    exit(0);
}

void WorkerChild::DoRun() {
    RunWorker();
    loop_->Run();
}

void WorkerChild::HandleChild() {
    int status = 0;
    pid_t pid = ::waitpid(-1, &status, WNOHANG);
    if (pid < 0) {
        PLOG(ERROR) << "waitpid()";
        return;
    } else if (pid == 0) {
        LOG(ERROR) << "No child was stopped";
        return;
    }
    CHECK(pid == worker_);
    stringstream ss;
    ss << "worker on " << pid << " ";
    if (WIFEXITED(status)) {
        ss << "exited with " << WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        ss << "killed by signal " << WTERMSIG(status);
    } else if (WCOREDUMP(status)) {
        ss << "core dumped";
    } else {
        LOG(ERROR) << "children is just stopped or continued";
        return;
    }
    LOG(WARNING) << ss.str() << ", rebooting...";
    worker_ = 0;

    timer_ = new net::TimerEvent(loop_);
    timer_->AsyncWait(
            boost::bind(&WorkerChild::RebootWorker, this),
            util::TimeDiff::Seconds(1));
}

void WorkerChild::RebootWorker() {
    delete timer_;
    timer_ = NULL;
    RunWorker();
}

void WorkerChild::DoCleanUp() {
    if (worker_ == 0) {
        return;
    }
    kill(worker_, SIGTERM);

    int64_t start = util::Time::CurrentMilliseconds();
    while (worker_ != 0) {
        int64_t now = util::Time::CurrentMilliseconds();
        if ((now - start) > kMaxWaitingWorker) {
            break;
        }
        int status = 0;
        pid_t pid = waitpid(-1, &status, WNOHANG);
        if (pid < 0) {
            PLOG(ERROR) << "waitpid()";
            break;
        } else if (pid == 0) {
            util::Thread::SleepForMs(10);
            continue;
        }
        CHECK_EQ(pid, worker_);
        if (WIFEXITED(status) || WIFSIGNALED(status) || WCOREDUMP(status)) {
            LOG(INFO) << "worker on " << pid << " finished";
            worker_ = 0;
        }
    }
    if (worker_ != 0) {
        kill(worker_, SIGKILL);
        LOG(ERROR) << "worker on " << worker_ << " refuse to die, killed";
    }
}

SingleMasterChild::SingleMasterChild(Config* config,
        net::EventLoop* event_loop,
        boost::ptr_map<int, ldd::net::Listener>& listeners)
    : Child(config, Module::kTypeSingleMaster),
      listeners_(listeners),
      mloop_(new net::EventLoop),
      wloop_(event_loop)
{
    set_title("master process");

    loop_ = mloop_;
}

SingleMasterChild::~SingleMasterChild() {
    delete wloop_;
}

bool SingleMasterChild::DoInit() {
    if (!module()) {
        LOG(ERROR) << "Invalid module object type";
        return false;
    }
    try {
        std::set<int> ports;
        boost::ptr_map<int, ldd::net::Listener>::iterator it
            = listeners_.begin();
        for (; it != listeners_.end(); ++it) {
            ports.insert(it->first);
        }
        SingleMasterModuleCtx ctx(config(), loop_, ports);
        LOG(INFO) << "Calling InitModule()";
        if (!module()->InitModule(&ctx)) {
            LOG(ERROR) << "Module init failed";
            return false;
        }
    } catch (const std::exception& e) {
        LOG(ERROR) << "Module init with exception: " << e.what();
        return false;
    } catch (...) {
        LOG(ERROR) << "Module init failed with unknown error";
        return false;
    }
    return true;
}

void SingleMasterChild::DoRun() {
    RunWorkers();
    loop_->Run();
}

void SingleMasterChild::RunWorkers() {
    typedef std::map<int, ldd::net::Listener*>::value_type
        PortAndListener;
    boost::ptr_map<int, ldd::net::Listener>::iterator it
        = listeners_.begin();
    for (; it != listeners_.end(); ++it) {
        RunWorker(it->first);
    }
}

void SingleMasterChild::RunWorker(int port) {
    CHECK(timers_.find(port) == timers_.end());

    google::FlushLogFiles(0);
    int fd[2];
    pipe(fd);
    char buf = 1;
    pid_t pid = fork();
    if (pid < 0) {
        PLOG(ERROR) << "fork worker failed";
        return;
    } else if (pid > 0) {
        // in master
        workers_[pid] = port;
        close(fd[0]);
        write(fd[1], &buf, 1);
        close(fd[1]);
        LOG(INFO) << "forked worker [" << port << "] on " << pid;
        return;
    }

    // in worker
    close(fd[1]);
    read (fd[0], &buf, 1);
    close(fd[0]);

    google::ShutdownGoogleLogging();
    std::string logname = "worker-" + lexical_cast<string>(port);
    google::InitGoogleLogging(logname.c_str());
    LOG(INFO) << "worker [" << port << "] started";

    // prepare and cleanup
    ListenerMap::iterator it = listeners_.find(port);
    CHECK(it != listeners_.end());
    net::Listener* listener = listeners_.release(it).release();
    listeners_.clear();
    listeners_.insert(port, listener);
    /*
    net::Listener* listener = &listeners_.at(port);
    */

    stringstream title;
    title << "lddskel(" << Option::instance().name() << "): worker process ["
            << port << "]";
    ProcTitle::Set(title.str());



    mloop_->AfterFork();
    wloop_->AfterFork();
    delete sigev_;
    delete mloop_;
    sigev_ = NULL;
    mloop_ = NULL;
    loop_ = wloop_;


    try {
        SingleMasterWorkerCtx ctx(listener, port);
        LOG(INFO) << "Calling InitWorker()";
        if (!module()->InitWorker(&ctx)) {
            LOG(ERROR) << "Worker init failed";
            goto finish;
        }
    } catch (const std::exception& e) {
        LOG(ERROR) << "Worker init with exception: " << e.what();
        goto finish;
    } catch (...) {
        LOG(ERROR) << "Worker init failed with unknown error";
        goto finish;
    }
    {
        LogFlusher flusher(loop_, config()->log_flush());
        loop_->Run();
    }
finish:
    //delete this;
    google::FlushLogFiles(0);
    exit(0);
}

void SingleMasterChild::HandleChild() {
    int status = 0;
    pid_t pid = ::waitpid(-1, &status, WNOHANG);
    if (pid < 0) {
        PLOG(ERROR) << "waitpid()";
        return;
    } else if (pid == 0) {
        LOG(ERROR) << "No child was stopped";
        return;
    }
    std::map<pid_t, int>::iterator it = workers_.find(pid);
    CHECK(it != workers_.end());
    int port = it->second;

    stringstream ss;
    ss << "worker [" << port << "] on " << pid << " ";
    if (WIFEXITED(status)) {
        ss << "exited with " << WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        ss << "killed by signal " << WTERMSIG(status);
    } else if (WCOREDUMP(status)) {
        ss << "core dumped";
    } else {
        LOG(ERROR) << "children is just stopped or continued";
        return;
    }
    LOG(WARNING) << ss.str() << ", rebooting...";
    workers_.erase(it);

    net::TimerEvent* timer = new net::TimerEvent(mloop_);
    timer->AsyncWait(
            boost::bind(&SingleMasterChild::RebootWorker, this, port),
            util::TimeDiff::Seconds(1));
    timers_[port] = timer;
}

void SingleMasterChild::RebootWorker(int port) {
    net::TimerEvent* timer = timers_[port];
    timers_.erase(port);
    delete timer;
    RunWorker(port);
}

void SingleMasterChild::DoCleanUp() {
    typedef std::map<pid_t, int>::value_type PidAndPort;
    BOOST_FOREACH(PidAndPort& x, workers_) {
        kill(x.first, SIGTERM);
    }
    int64_t start = util::Time::CurrentMilliseconds();
    while (!workers_.empty()) {
        int64_t now = util::Time::CurrentMilliseconds();
        if ((now - start) > kMaxWaitingWorker) {
            break;
        }
        int status = 0;
        pid_t pid = waitpid(-1, &status, WNOHANG);
        if (pid < 0) {
            PLOG(ERROR) << "waitpid()";
            break;
        } else if (pid == 0) {
            util::Thread::SleepForMs(100);
            continue;
        }
        std::map<pid_t, int>::iterator it = workers_.find(pid);
        CHECK(it != workers_.end());
        int port = it->second;
        if (WIFEXITED(status) || WIFSIGNALED(status) || WCOREDUMP(status)) {
            LOG(INFO) << "worker [" << port << "] on " << pid << " finished";
            workers_.erase(pid);
            continue;
        }
    }
    BOOST_FOREACH(PidAndPort& x, workers_) {
        int port = x.second;
        pid_t pid = x.first;
        kill(x.first, SIGKILL);
        LOG(ERROR) << "worker [" << port << "] on "
            << pid << " refuse to die, killed";
    }
}

} // namespace skel
} // namespace ldd
