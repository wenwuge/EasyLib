#ifndef LDD_CAGE_ZOOKEEPER_IMPL_H_
#define LDD_CAGE_ZOOKEEPER_IMPL_H_

#include <boost/noncopyable.hpp>
#include <vector>
#include <map>
#include "ldd/cage/keeper.h"
#include "functions.h"

namespace ldd {
namespace cage {

typedef std::vector<NodeWatcher> NodeWatchers;
typedef std::vector<ChildWatcher> ChildWatchers;
typedef std::map<std::string, NodeWatchers> NodeWatchersMap;
typedef std::map<std::string, ChildWatchers> ChildWatchersMap;

class Keeper::Impl : boost::noncopyable {
public:
    Impl(net::EventLoop* event_loop, const KeeperListener& listener);
    ~Impl();

    bool IsOpen() const;
    bool IsUnrecoverable() const;
    int timeout() const;

    bool Open(const std::string& dest, int timeout);
    void Close();

    void AddAuth(const std::string& scheme, const std::string& cert,
            const AddAuthCallback& callback);

    void Create(const std::string& path, const std::string& value,
            const std::vector<Acl>& acls, Mode::Type mode,
            const CreateCallback& callback);

    void Delete(const std::string& path, int32_t version,
            const DeleteCallback& callback);

    void Exists(const std::string& path,
            const NodeWatcher& watcher,
            const ExistsCallback& callback);

    void Get(const std::string& path,
            const NodeWatcher& watcher,
            const GetCallback& callback);

    void Set(const std::string& path, const std::string& value,
            int32_t version, const SetCallback& callback);

    void GetAcl(const std::string& path, const GetAclCallback& callback);

    void SetAcl(const std::string& path, const std::vector<Acl>& acls,
            int32_t version, const SetAclCallback& callback);

    void GetChildren(const std::string& path,
            const ChildWatcher& watcher,
            const GetChildrenCallback& callback);

    void GetChildrenWithStat(const std::string& path,
            const ChildWatcher& watcher,
            const GetChildrenWithStatCallback& callback);

    void Multi(const std::vector<Op*>& ops, const MultiCallback& callback);

private:
    friend void WatchSession(zhandle_t*, int, int, const char*, void*);
    friend void WatchNode(zhandle_t*, int, int, const char*, void*);
    friend void WatchChild(zhandle_t*, int, int, const char*, void*);

    Status Interest(int* fd, int* interest, ldd::util::TimeDiff* timeout);
    Status Process(int events);
    void UpdateEvent();
    void ClearEvent();
    void HandleEvent(int events);

    mutable zhandle_t* zh_;
    net::EventLoop* event_loop_;
    net::FdEvent event_;
    KeeperListener listener_;
    NodeWatchersMap node_watcher_;
    ChildWatchersMap child_watcher_;
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_ZOOKEEPER_IMPL_H_
