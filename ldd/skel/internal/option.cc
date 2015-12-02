// option.cc (2013-07-31)
// Li Xinjie (xjason.li@gmail.com)

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>
#include <string.h>
#include <string.h>
#include <iostream>
#include "option.h"

namespace ldd {
namespace skel {

using namespace std;
using namespace boost;

namespace po = boost::program_options;

const char* kCfgExt = ".conf";
const char* kPidExt = ".pid";
const char* kDefaultCfgDir = "/home/s/etc/lddskel";
const char* kDefaultLibDir = "/home/s/lib/lddskel";
const char* kDefaultLogDir = "/home/s/var/log";
const char* kDefaultPidDir = "/home/s/var/run";

Option Option::instance_;


Option::Option()
    : kill_(false), reload_(false), status_(false), foreground_(false)
{
}

Option::~Option() {}

bool Option::Init(int argc, char** argv) {
    std::string log_dir;
    std::string pid_dir;
    std::string cfg_dir;
    std::string lib_dir;

    bool auto_name = false;
    char* argv0 = strdup(argv[0]);
    char* prog = basename(argv0);
    if (strlen(prog) && strcmp(prog, "lddskel") != 0) {
        name_ = prog;
        auto_name = true;
    }
    free(argv0);

    po::options_description generic("Generic options");
    if (!auto_name) {
        generic.add_options()
            ("name,n", po::value<string>(&name_), "skel name [REQUIRED]");
    }
    generic.add_options()
        ("help,h", "show this message")
        ("kill,k", "kill the running skel process")
        ("reload,r", "reload the running skel process")
        ("status,s", "check skel process status")
        ("foreground,f", "run in foreground")
        ("log-dir",
            po::value<string>(&log_dir)->default_value(kDefaultLogDir),
            "log file directory")
        ("pid-dir",
            po::value<string>(&pid_dir)->default_value(kDefaultPidDir),
            "pid file directory")
        ("cfg-dir",
            po::value<string>(&cfg_dir)->default_value(kDefaultCfgDir),
            "config file directory")
        ("lib-dir",
            po::value<string>(&lib_dir)->default_value(kDefaultLibDir),
            "module library file directory");

    po::options_description config("Configure options");
    MakeConfigs(&config);

    po::options_description cmdline_options;
    cmdline_options.add(generic).add(config);

    try {
        po::store(po::parse_command_line(argc, argv, cmdline_options), vm_);
    } catch (const std::exception& e) {
        cerr << "Error parsing args: " << e.what() << endl;
        return false;
    }
    po::notify(vm_);

    if (vm_.count("help")) {
        cout << "Usage: " << argv[0];
        if (!auto_name) {
            cout << " -n <name> [-k|-r|-s] [options...]";
        } else {
            cout << " [-k|-r|-s] [options...]";
        }
        cout << endl << cmdline_options << endl;
        return false;
    }
    if (name_.empty()) {
        cerr << "Option '-n' is required" << endl;
        cout << cmdline_options << endl;
        return false;
    }
    if (vm_.count("kill")) {
        kill_ = true;
    }
    if (vm_.count("reload")) {
        reload_ = true;
    }
    if (vm_.count("status")) {
        status_ = true;
    }

    if ((kill_ && reload_) || (kill_ && status_)
            || (reload_ && status_)) {
        cerr << "Option '-r|-k|-s can't be used together" << endl;
        return false;
    }

    if (vm_.count("foreground")) {
        foreground_ = true;
    }

    log_dir_ = log_dir + "/";
    lib_dir_ = lib_dir + "/";
    cfg_file_ = cfg_dir + "/" + name_ + kCfgExt;
    pid_file_ = pid_dir + "/" + name_ + kPidExt;

    umask(002);
    mkdir(log_dir_.c_str(), 0775);
    umask(022);
    struct stat stbuf;
    if (stat(log_dir_.c_str(), &stbuf) < 0) {
        cerr << "Can't open log-dir " << log_dir_ << ": " << strerror(errno);
        return false;
    }
    if (!S_ISDIR(stbuf.st_mode)) {
        cerr << "Invalid --log-dir " << log_dir_ << ": not a directory";
        return false;
    }

    string logdir  = log_dir_ + name_;
    umask(002);
    mkdir(logdir.c_str(), 0775);
    umask(022);
    int saved_errno = errno;
    if (stat(logdir.c_str(), &stbuf) < 0) {
        cerr << "Can't write log-dir " << log_dir_ << ": "
            << strerror(saved_errno);
        return false;
    }
    if (!S_ISDIR(stbuf.st_mode)) {
        cerr << logdir << " is not a directory";
        return false;
    }
    log_dir_ = logdir;

    return true;
}

void Option::MakeConfigs(po::options_description* options) {
    options->add_options()
        ("port", po::value<string>(), "listen port(s), separated by ','")
        ("addr",
            po::value<string>()->default_value("0.0.0.0"),
            "listen ip address, or can be special values below:\n"
            "* any       - equal to 0.0.0.0\n"
            "* localhost - equal to 127.0.0.1\n"
            "* lan       - find a lan address to listen on\n"
            "* wan       - find a wan address to listen on\n")
        ("user", po::value<string>(),
            "the name of the user to run this module, may need root "
            "privilege\n")
        ("file",
            po::value<string>(),
            "module library file, searching from $(lib-dir) if it is "
            "a relative path, default value is '$(name).so'\n")
        ("type",
            po::value<string>()->default_value("simple"),
            "module type, should be one of:\n"
            "* simple        - each port fork a child process\n"
            "* worker        - each port fork a child process, then fork a"
                             " worker process to handle connection\n"
            "* single-master - create only 1 child process, then for each"
                             " port, fork a worker process to handle"
                             " connection\n")
        ("reload-policy",
            po::value<string>()->default_value("simple"),
            "reload policy, shoud be one of:\n"
            "* simple - kill the old process, and start the new one\n"
            "* safe   - old process will be stopped after the "
                            "new one is successfully started\n"
            "* sole   - old process will be stopped before "
                            "starting the new one\n")
        ("init-timeout",
            po::value<int>()->default_value(kDefaultInitTimeout),
            "module initialie timeout in seconds, 0 or negative is invalid\n")
        ("stop-timeout",
            po::value<int>()->default_value(kDefaultStopTimeout),
            "module stopping timeout in seconds, 0 or negative is invalid\n")
        ("reboot-delay",
            po::value<int>()->default_value(kDefaultRebootDelay),
            "module reboot delay seconds\n")
        ("auto-restart",
            po::value<bool>()->default_value(true),
            "auto restart stopped child processes\n")

        ("log-level", po::value<int>()->default_value(0),
            "log level, 0=INFO 1=WARNING 2=ERROR 3=FATAL")
        ("log-flush", po::value<int>()->default_value(10),
            "log flush interval, in seconds")
        ("log-size", po::value<int>()->default_value(10),
            "log size max, in MB");
}


} // namespace skel
} // namespace ldd
