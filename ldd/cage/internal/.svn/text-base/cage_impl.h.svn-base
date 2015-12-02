// cage_impl.h (2013-09-12)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_CAGE_INTERNAL_CAGE_IMPL_H_
#define LDD_CAGE_INTERNAL_CAGE_IMPL_H_

#include <string>
#include <set>
#include <boost/intrusive/list.hpp>
#include <ldd/net/event_loop.h>
#include <ldd/net/event.h>
#include "ldd/cage/keeper.h"
#include "ldd/cage/callback.h"
#include "ldd/cage/cage.h"
#include "retry_loop.h"

namespace ldd {
namespace cage {

class RetryPolicy;

class Cage::Impl {
public:
    Impl(net::EventLoop* event_loop, const CageListener& listener,
            RetryPolicy* policy = NULL);
    ~Impl();

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

    net::EventLoop* event_loop() const { return event_loop_; }

private:
    void ListenKeeper(const KeeperState&);
    void Lost();
    void SuspendTimeout();
    void NotifyListener();
    friend class RetryLoop;
private:
    net::EventLoop* event_loop_;
    CageState state_;
    Keeper keeper_;
    net::TimerEvent timer_;
    CageListener listener_;
    util::TimeDiff timeout_;
    RetryPolicy* policy_;
    RetryList retries_;
};
    
} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_INTERNAL_CAGE_IMPL_H_
