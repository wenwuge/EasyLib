// cage.h (2013-09-06)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_CAGE_CAGE_H_
#define LDD_CAGE_CAGE_H_

#include <string>
#include <set>
#include <ldd/net/event_loop.h>
#include <ldd/net/event.h>
#include "ldd/cage/callback.h"
#include "ldd/cage/mode.h"

namespace ldd {
namespace cage {

class RetryLoop;
class RetryPolicy;

class Cage {
public:
    class Impl;
    Cage(net::EventLoop* event_loop, const CageListener& listener,
            RetryPolicy* policy = NULL);
    ~Cage();

    bool Open(const std::string& dest, int timeout);
    void Close();

    void AddAuth(const std::string& scheme, const std::string& cert,
            const AddAuthCallback& callback);

    void Create(const std::string& path, const std::string& data,
            const std::vector<Acl>& acls, Mode::Type mode,
            const CreateCallback& callback);

    void Delete(const std::string& path, int32_t version,
            const DeleteCallback& callback);

    void Exists(const std::string& path, const NodeWatcher& watcher,
            const ExistsCallback& callback);

    void Get(const std::string& path, const NodeWatcher& watcher,
            const GetCallback& callback);

    void Set(const std::string& path, const std::string& data,
            int32_t version, const SetCallback& callback);

    void GetAcl(const std::string& path, const GetAclCallback& callback);

    void SetAcl(const std::string& path, const std::vector<Acl>& acls,
            int32_t version, const SetAclCallback& callback);

    void GetChildren(const std::string& path, const ChildWatcher& watcher,
            const GetChildrenCallback& callback);

    void GetChildrenWithStat(const std::string& path,
            const ChildWatcher& watcher,
            const GetChildrenWithStatCallback& callback);

    void Multi(const std::vector<Op*>& ops, const MultiCallback& callback);

    net::EventLoop* event_loop() const;

private:
    Impl* impl_;
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_CAGE_H_
