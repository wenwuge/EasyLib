// config.cc (2013-07-26)
// Li Xinjie (xjason.li@gmail.com)

#include <ifaddrs.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/types.h>
#include <pwd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <glog/logging.h>
#include "ldd/skel/module.h"
#include "config.h"
#include "option.h"


namespace ldd {
namespace skel {


using namespace std;
using namespace boost;
namespace po = boost::program_options;

static bool ParsePortList(const string& list, set<int>* ports, string* what);
static bool ParsePortField(const string& field, set<int>* ports);
static int ParsePortValue(const string& value);
static bool ParseAddr(const std::string& config, net::Address* addr,
        bool* valid);
static bool ParseType(const std::string& config, Module::Type* type);
static bool ParseReloadPolicy(const std::string& config,
        Reload::Policy* policy);
static bool CheckUser(const std::string& user, std::string* what);

Config::Config()
    : type_(Module::kTypeSimple),
      reload_policy_(Reload::kPolicySimple),
      init_timeout_(0),
      stop_timeout_(0),
      reboot_delay_(0),
      auto_restart_(true),
      log_level_(0),
      log_flush_(0),
      log_size_(0)
{
}

Config::~Config() {
}

bool Config::Init(std::string* what) {
    Option& option = Option::instance();
    po::variables_map vm(option.vm());

    ifstream ifs(option.cfg_file().c_str());
    if (!ifs.is_open()) {
        stringstream ss;
        ss << "Can't open " << option.cfg_file() << ": " << strerror(errno);
        *what = ss.str();
        return false;
    }

    po::options_description config;
    option.MakeConfigs(&config);

    try {
        po::parsed_options parsed_options =
            po::parse_config_file(ifs, config, true);
        po::store(parsed_options, vm);
        if (!vm.count("port")) {
            *what = "Config 'port' is required";
            return false;
        }
        bool addr_is_valid = false;
        if (!ParseAddr(vm["addr"].as<string>(), &addr_, &addr_is_valid)) {
            *what = "Config 'addr' is invalid";
            return false;
        }
        if (!addr_is_valid) {
            *what = "Can't find valid '"
                + vm["addr"].as<string>() + "' address";
            return false;
        }
        if (!ParsePortList(vm["port"].as<string>(), &ports_, what)) {
            return false;
        }

        if (vm.count("user")) {
            user_ = vm["user"].as<string>();
            if (!CheckUser(user_, what)) {
                return false;
            }
        }

        if (!vm.count("file")) {
            file_ = option.name() + ".so";
        } else {
            file_ = vm["file"].as<string>();
            if (file_.empty()) {
                *what =  "Config 'file' is empty";
                return false;
            }
        }
        if (file_[0] != '/') {
            file_ = option.lib_dir() + file_;
            int fd = -1;
            if ((fd = open(file_.c_str(), O_RDONLY)) < 0) {
                stringstream ss;
                ss << "Can't open file " << file_.c_str()
                    << ": " << strerror(errno);
                *what = ss.str();
                return false;
            }
            close(fd);
        }

        if (!ParseType(vm["type"].as<string>(), &type_)) {
            *what = "Config 'type' is invalid";
            return false;
        }
        if (!ParseReloadPolicy(
                    vm["reload-policy"].as<string>(), &reload_policy_)) {
            *what = "Config 'reload-policy' is invalid";
            return false;
        }
        init_timeout_ = vm["init-timeout"].as<int>();
        if (init_timeout_ <= 0) {
            init_timeout_ = kDefaultInitTimeout;
        }
        stop_timeout_ = vm["stop-timeout"].as<int>();
        if (stop_timeout_ <= 0) {
            stop_timeout_ = kDefaultStopTimeout;
        }
        reboot_delay_ = vm["reboot-delay"].as<int>();
        if (reboot_delay_ <= 0) {
            reboot_delay_ = kDefaultRebootDelay;
        }

        auto_restart_ = vm["auto-restart"].as<bool>();

        log_level_ = vm["log-level"].as<int>();
        log_flush_ = vm["log-flush"].as<int>();
        if (log_flush_ <= 0) {
            log_flush_ = 10;
        }
        log_size_ = vm["log-size"].as<int>();
        if (log_size_ <= 0) {
            log_size_ = 10;
        }
        

        BOOST_FOREACH(po::option opt, parsed_options.options) {
            if (!opt.unregistered) {
                continue;
            }
            string key = opt.string_key;
            size_t pos = key.find('.');
            if (pos == string::npos || pos == 0
                    || pos == (key.size() - 1)) {
                stringstream ss;
                ss << "Unknown config section '" << key << "'";
                *what = ss.str();
                return false;
            }
            if (opt.value.size() != 1) {
                stringstream ss;
                ss << "Duplicate config section '" << key << "'";
                *what = ss.str();
                return false;
            }
            properties_.add(key, opt.value[0]);
        }
        return true;
    } catch (const std::exception& e) {
        stringstream ss;
        ss << "Can't Parse " << option.cfg_file() << ": " << e.what();
        *what = ss.str();
        return false;
    }
    
    return true;
}

bool ParsePortList(const std::string& list,
        std::set<int>* ports, std::string* what) {
    stringstream ss;
    string v = trim_copy(list);
    vector<string> fields;
    boost::split(fields, v, boost::is_any_of(","), boost::token_compress_on);
    bool has_port = false;
    BOOST_FOREACH(string& field, fields) {
        trim(field);
        if (field.empty()) {
            continue;
        }
        if (!ParsePortField(field, ports)) {
            ss << "Error parsing port '" << field << "'";
            *what = ss.str();
            return false;
        }
        has_port = true;
    }
    if (!has_port) {
        ss << "Error parsing port list, it's empty";
        *what = ss.str();
        return false;
    }
    return true;
}

bool ParsePortField(const std::string& field,
        std::set<int>* ports) {
    size_t pos = field.find('-');
    if (pos == string::npos) {
        int port = ParsePortValue(field);
        if (port <= 0) {
            return false;
        }
        ports->insert(port);
        return true;
    }
    if (pos == 0 || (pos == field.size() - 1)) {
        return false;
    }
    string v0 = field.substr(0, pos);
    string v1 = field.substr(pos + 1);
    int port0 = ParsePortValue(v0);
    int port1 = ParsePortValue(v1);
    if (port0 <= 0 || port1 <= 0) {
        return false;
    }
    if (port0 > port1) {
        std::swap(port0, port1);
    }
    for (int i = port0; i <= port1; ++i) {
        ports->insert(i);
    }
    return true;
}

int ParsePortValue(const std::string& value) {
    int port = 0;
    try {
        port = lexical_cast<int>(value);
    } catch(bad_lexical_cast const&) {}
    return port;
}

static bool IsPrivateAddr(const net::Address& addr) {
    if (addr.IsClassA()) {
        if ((addr.ToU32() & 0xFF000000) == 0x0A000000) {
            return true;
        }
    } else if (addr.IsClassB()) {
        if ((addr.ToU32() & 0xFFF00000) == 0xAC100000) {
            return true;
        }
    } else if (addr.IsClassC()) {
        if ((addr.ToU32() & 0xFFFF0000) == 0xC0A80000) {
            return true;
        }
    }
    return false;
}

static bool IsReservedAddr(const net::Address& addr) {
    if (addr.IsClassA()) {
        if ((addr.ToU32() & 0xFF000000) == 0x7F000000) {
            return true;
        }
    } else if (addr.IsClassB()) {
        if ((addr.ToU32() & 0xFFFF0000) == 0xA9FE0000) {
            return true;
        }
    } else if (addr.IsUnspecified() || addr.IsMulticast() 
            || addr.IsLoopback()) {
        return true;
    }
    return false;
}

static void GetAllAddr(std::vector<net::Address>* addrs) {
    struct ifaddrs* ifaddrs_list = NULL;
    struct ifaddrs* ifa = NULL;
    if (getifaddrs(&ifaddrs_list)) {
        return;
    }
    for (ifa = ifaddrs_list; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family == AF_INET) {
            net::Address addr(((struct sockaddr_in*)ifa->ifa_addr)->sin_addr);
            addrs->push_back(addr);
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            // ipv6 not supported yet
        }
    }
    freeifaddrs(ifaddrs_list);
}

static bool GetLanAddr(net::Address* addr) {
    std::vector<net::Address> addrs;
    GetAllAddr(&addrs);
    BOOST_FOREACH(const net::Address& a, addrs) {
        if (IsPrivateAddr(a)) {
            *addr = a;
            return true;
        }
    }
    return false;
}

static bool GetWanAddr(net::Address* addr) {
    std::vector<net::Address> addrs;
    GetAllAddr(&addrs);
    BOOST_FOREACH(const net::Address& a, addrs) {
        if (!IsPrivateAddr(a) && !IsReservedAddr(a)) {
            *addr = a;
            return true;
        }
    }
    return false;
}

bool ParseAddr(const std::string& v, net::Address* addr, bool* valid) {
    if (v == "any" || v == "0.0.0.0") {
        *valid = true;
        *addr = net::Address();
    } else if (v == "lan") {
        *valid = GetLanAddr(addr);
    } else if (v == "wan") {
        *valid = GetWanAddr(addr);
    } else if (v == "localhost" || v == "127.0.0.1") {
        *valid = true;
        *addr = net::Address::Loopback(net::Address::V4);
    } else {
        *addr = net::Address::FromString(v);
        if (addr->IsUnspecified()) {
            return false;
        }
        *valid = true;
    }
    return true;
}

bool ParseType(const std::string& v, Module::Type* type) {
    if (v == "simple") {
        *type = Module::kTypeSimple;
        return true;
    } else if (v == "worker") {
        *type = Module::kTypeWorker;
        return true;
    } else if (v == "single-master") {
        *type = Module::kTypeSingleMaster;
        return true;
    }
    return false;
}

bool ParseReloadPolicy(const std::string& v, Reload::Policy* policy) {
    if (v == "simple" ) {
        *policy = Reload::kPolicySimple;
        return true;
    } else if (v == "safe") {
        *policy = Reload::kPolicySafe;
        return true;
    } else if (v == "sole") {
        *policy = Reload::kPolicySole;
        return true;
    }
    return false;
}

bool CheckUser(const std::string& user, std::string* what) {
    if (user.empty()) {
        return true;
    }
    errno = 0;
    struct passwd* pw = getpwnam(user.c_str());
    if (!pw) {
        if (errno == 0) {
            *what = user + " can not found in /etc/passwd";
        } else {
            *what = std::string("getpwnam: ") + strerror(errno);
        }
        return false;
    }
    if (getuid() == 0 || geteuid() == 0) {
        return true;
    }
    if (pw->pw_uid == getuid() || pw->pw_uid == geteuid()) {
        return true;
    }
    stringstream ss;
    ss << "current uid=" << getuid()
        << " don't have privilege to change uid to " << pw->pw_uid;
    *what = ss.str();
    return false;
}

} // namespace skel
} // namespace ldd
