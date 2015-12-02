// skel.cc (2013-07-18)
// Li Xinjie (xjason.li@gmail.com)

#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>
#include <iostream>
#include <boost/scoped_array.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <glog/logging.h>
#include <libdaemon/daemon.h>
#include "skel.h"
#include "option.h"
#include "proctitle.h"
#include "monitor.h"

namespace ldd {
namespace skel {

using namespace std;
using namespace boost;

static const char* PidFileName() {
    return Option::instance().pid_file().c_str();
}

Skel::Skel()
    : monitor_(NULL)
{
}

Skel::~Skel() {
    delete monitor_;
}

bool Skel::Init(int argc, char** argv) {
    Option& option = Option::instance();
    if (!option.Init(argc, argv)) {
        return false;
    }
    if (option.kill()) {
        KillDaemon();
        return false;
    }
    if (option.reload()) {
        ReloadDaemon();
        return false;
    }
    if (option.status()) {
        CheckStatus();
        return false;
    }
    monitor_ = new Monitor;
    if (!monitor_->Init(argc, argv)) {
        return false;
    }
    return true;
}

void Skel::Run() {
    Option& option = Option::instance();
    if (option.foreground()) {
        RunForeground();
    } else {
        RunAsDaemon();
    }
}

void Skel::RunMonitor() {
    Option& option = Option::instance();
    FLAGS_log_dir = option.log_dir();
    FLAGS_stderrthreshold = 0;
    google::InitGoogleLogging("monitor");
    monitor_->Run();
}

void Skel::RunForeground() {
    RunMonitor();
}

void Skel::RunAsDaemon() {
    Option& option = Option::instance();
    daemon_log_ident = option.name().c_str();
    daemon_pid_file_proc = PidFileName;

    pid_t pid = daemon_pid_file_is_running();
    if (pid >= 0) {
        daemon_log(LOG_ERR, "lddskel(%s) is already running on %d",
                option.name().c_str(), pid);
        exit(1);
    }

    if (daemon_retval_init() != 0) {
        daemon_log(LOG_ERR, "daemonize failed");
        exit(1);
    }

    pid = daemon_fork();
    if (pid < 0) {
        daemon_log(LOG_ERR, "daemonize failed: %s", strerror(errno));
        daemon_retval_done();
        exit(1);
    } else if (pid > 0) {
        // in parent
        int retval = 0;
        if ((retval = daemon_retval_wait(10)) < 0) {
            daemon_log(LOG_ERR, "lddskel(%s) can't get daemon retval: %s",
                    option.name().c_str(), strerror(errno));
            exit(1);
        }
        switch (retval) {
        case 1:
            daemon_log(LOG_ERR, "lddskel(%s) can't close all fds",
                    option.name().c_str());
            break;
        case 2:
            daemon_log(LOG_ERR, "lddskel(%s) can't write pid file %s",
                    option.name().c_str(),
                    option.pid_file().c_str());
            break;
        default:
            daemon_log(LOG_INFO, "lddskel(%s) started on %d",
                    option.name().c_str(), pid);
        }
        exit(retval);
    } else {
        // in daemon
        int retval = 0;
        if (daemon_close_all(-1) != 0) {
            daemon_log(LOG_ERR, "can't close all fds: %s", strerror(errno));
            retval = 1;
            goto finish;
        }
        if (daemon_pid_file_create() != 0) {
            daemon_log(LOG_ERR, "can't write pid file %s: %s",
                    option.pid_file().c_str(), strerror(errno));
            retval = 2;
            goto finish;
        }
        daemon_retval_send(0);
        daemon_log(LOG_INFO, "daemon start ok");

        umask(022);

        RunMonitor();
finish:
        if (retval != 0) {
            daemon_retval_send(retval);
        }
        daemon_log(LOG_INFO, "daemon exiting...");
        daemon_signal_done();
        daemon_pid_file_remove();
    }
}

void Skel::KillDaemon() {
    Option& option = Option::instance();
    daemon_pid_file_proc = PidFileName;

    pid_t pid = daemon_pid_file_is_running();
    if (pid >= 0) {
        cout << "lddskel(" << option.name() << ") is running on "
            << pid << endl;
        if (daemon_pid_file_kill_wait(SIGTERM, 5)) {
            cout << "can't kill: " << strerror(errno) << endl;
            exit(1);
        }
        cout << "killed" << endl;
        exit(0);
    } else {
        cout << "lddskel(" << option.name() << ") is not running" << endl;
        exit(1);
    }
}

void Skel::ReloadDaemon() {
    Option& option = Option::instance();
    daemon_pid_file_proc = PidFileName;

    pid_t pid = daemon_pid_file_is_running();
    if (pid >= 0) {
        cout << "lddskel(" << option.name() << ") is running on "
            << pid << endl;
        if (daemon_pid_file_kill(SIGHUP)) {
            cout << "can't reload: " << strerror(errno) << endl;
            exit(1);
        }
        cout << "reloaded" << endl;
        exit(0);
    } else {
        cout << "lddskel(" << option.name() << ") is not running" << endl;
        exit(1);
    }
}

void Skel::CheckStatus() {
    Option& option = Option::instance();
    daemon_pid_file_proc = PidFileName;

    pid_t pid = daemon_pid_file_is_running();
    if (pid >= 0) {
        cout << "lddskel(" << option.name() << ") is running on "
            << pid << endl;
        exit(0);
    } else {
        cout << "lddskel(" << option.name() << ") is not running" << endl;
        exit(1);
    }
}

} // namespace skel
} // namespace ldd

