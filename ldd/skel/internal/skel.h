// skel.h (2013-07-18)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_SKEL_SKEL_H_
#define LDD_SKEL_SKEL_H_

#include <string>

namespace ldd {
namespace skel {

class Monitor;

class Skel {
public:
    Skel();
    ~Skel();

    bool Init(int argc, char** argv);
    void Run();
private:
    void RunForeground();
    void RunAsDaemon();
    void RunMonitor();
    void KillDaemon();
    void ReloadDaemon();
    void CheckStatus();

    Monitor* monitor_;
};

} // namespace skel
} // namespace ldd

#endif // LDD_SKEL_SKEL_H_
