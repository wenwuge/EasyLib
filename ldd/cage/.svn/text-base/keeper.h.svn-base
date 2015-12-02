#ifndef LDD_CAGE_ZOOKEEPER_H_
#define LDD_CAGE_ZOOKEEPER_H_

#include <string>
#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>
#include <zookeeper/zookeeper.h>
#include <ldd/net/event_loop.h>
#include <ldd/net/event.h>
#include <ldd/util/time_diff.h>
#include "ldd/cage/acl.h"
#include "ldd/cage/mode.h"
#include "ldd/cage/stat.h"
#include "ldd/cage/status.h"
#include "ldd/cage/callback.h"

namespace ldd {
namespace cage {

class Keeper {
public:
    class Impl;
    Keeper(net::EventLoop* event_loop, const KeeperListener& listener);
    ~Keeper();

    bool IsOpen() const;
    bool IsUnrecoverable() const;
    int timeout() const;

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

private:
    // Disable copying
    Keeper(const Keeper&);
    Keeper& operator = (const Keeper&);

    Impl* impl_;
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_ZOOKEEPER_H_
