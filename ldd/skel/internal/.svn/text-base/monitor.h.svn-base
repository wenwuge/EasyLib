// monitor.h (2013-07-26)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_SKEL_MONITOR_H_
#define LDD_SKEL_MONITOR_H_

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <ldd/util/atomic.h>
#include <ldd/net/event_loop.h>
#include <ldd/net/listener.h>
#include <ldd/net/event.h>
#include "config.h"

namespace ldd {
namespace skel {

typedef boost::ptr_map<int, net::Listener> ListenerMap;

class LogFlusher;

class Monitor {
public:
    Monitor();
    ~Monitor();
    bool Init(int argc, char** argv);
    void Run();
private:
    void RunMasters();
    void RunMaster(int port);
    void RunMaster();
    void WatchSignals();
    void Loop();
    void StopChildren();
    void StopChild();
    void HandleSignal(int sig);
    void HandleStop();
    void HandleChild();
    void HandleChildren();
    void HandleReload();
    void ChangeUser();
    void DoReload();
    void ReloadSingleChild();
    void ReloadMultiChild();
    //void OnReloadSingleSoleTimeout();
    //void OnReloadMultiSoleTimeout();

    void DropChild(pid_t pid, int port = 0);
    void OnDropChildTimeout(pid_t pid);
    bool IsDropped(pid_t pid, int* port = NULL);
    void ClearDropped(pid_t pid);
    void OnSingleMasterInit(int ev, int fd);

    boost::shared_ptr<Config> config_;
    boost::scoped_ptr<net::EventLoop> mloop_;
    boost::scoped_ptr<net::EventLoop> cloop_;
    boost::scoped_ptr<net::SignalEvent> sigev_;
    boost::ptr_map<int, net::Listener> listeners_;
    boost::scoped_ptr<LogFlusher> flusher_;

    // Single child
    pid_t child_;
    pid_t new_child_;
    boost::scoped_ptr<net::TimerEvent> reboot_timer_;
    boost::scoped_ptr<net::FdEvent> event_;

    // Multi child
    std::map<pid_t, int> children_;
    boost::ptr_map<int, net::TimerEvent> reboot_timers_;

    // Reload
    boost::shared_ptr<Config> rconfig_;

    // killed children
    boost::ptr_map<pid_t, net::TimerEvent> dropped_children_;
    std::map<pid_t, int> dropped_ports_;
};

} // namespace skel
} // namespace ldd

#endif // LDD_SKEL_MONITOR_H_
