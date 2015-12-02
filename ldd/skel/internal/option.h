// option.h (2013-07-31)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_SKEL_OPTION_H_
#define LDD_SKEL_OPTION_H_

#include <boost/noncopyable.hpp>
#include <boost/program_options.hpp>

namespace ldd {
namespace skel {

const int kDefaultInitTimeout = 5;
const int kDefaultStopTimeout = 5;
const int kDefaultRebootDelay = 1;


class Option : boost::noncopyable {
public:
    Option();
    ~Option();

    bool Init(int argc, char** argv);

    const std::string& name() const { return name_; }
    bool kill() const { return kill_; }
    bool reload() const { return reload_; }
    bool status() const { return status_; }
    bool foreground() const { return foreground_; }
    const std::string& log_dir() const { return log_dir_; }
    const std::string& lib_dir() const { return lib_dir_; }
    const std::string& pid_file() const { return pid_file_; }
    const std::string& cfg_file() const { return cfg_file_; }
    
    const boost::program_options::variables_map&
        vm() const { return vm_; }

    void MakeConfigs(
            boost::program_options::options_description* options);

    static Option& instance() { return instance_; }
private:
    boost::program_options::variables_map vm_;

    // Options only init with cmdline.
    std::string name_;
    bool kill_;
    bool reload_;
    bool status_;
    bool foreground_;
    std::string log_dir_;
    std::string lib_dir_;
    std::string pid_file_;
    std::string cfg_file_;

    static Option instance_;
};

} // namespace skel
} // namespace ldd

#endif // LDD_SKEL_OPTION_H_
