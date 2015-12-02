// config.h (2013-07-26)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_SKEL_CONFIG_H_
#define LDD_SKEL_CONFIG_H_

#include <set>
#include "ldd/skel/module.h"
#include "ldd/net/address.h"

namespace ldd {
namespace skel {

using boost::property_tree::ptree;

struct Reload {
    enum Policy {
        kPolicySimple,
        kPolicySafe,
        kPolicySole,
    };
};


class Config {
public:
    explicit Config();
    ~Config();

    bool Init(std::string* what);

    const std::string& file() const { return file_; }
    const std::set<int>& ports() const { return ports_; }
    const net::Address& addr() const { return addr_; }
    const std::string& user() const { return user_; }
    Module::Type type() const { return type_; }
    Reload::Policy reload_policy() const { return reload_policy_; }
    int init_timeout() const { return init_timeout_; }
    int stop_timeout() const { return stop_timeout_; }
    int reboot_delay() const { return reboot_delay_; }
    bool auto_restart() const { return auto_restart_; }
    int log_level() const { return log_level_; }
    int log_flush() const { return log_flush_; }
    int log_size() const { return log_size_; }

    const ptree& properties() const { return properties_; }
private:
    std::string file_;
    std::set<int> ports_;
    std::string user_;
    net::Address addr_;
    Module::Type type_;
    Reload::Policy reload_policy_;
    int init_timeout_;
    int stop_timeout_;
    int reboot_delay_;
    bool auto_restart_;
    int log_level_;
    int log_flush_;
    int log_size_;
    ptree properties_;
};

} // namespace skel
} // namespace ldd

#endif // LDD_SKEL_CONFIG_H_
