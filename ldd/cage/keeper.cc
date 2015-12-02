#include <boost/scoped_array.hpp>
#include <boost/bind.hpp>
#include <glog/logging.h>
#include <zookeeper/zookeeper.h>
#include "keeper.h"
#include "internal/keeper_impl.h"
#include <ldd/util/time_diff.h>

namespace ldd {
namespace cage {

Keeper::Keeper(net::EventLoop* event_loop, const KeeperListener& listener)
    : impl_(new Impl(event_loop, listener))
{
}

Keeper::~Keeper()
{
    delete impl_;
}

bool Keeper::IsOpen() const {
    return impl_->IsOpen();
}

bool Keeper::IsUnrecoverable() const {
    return impl_->IsUnrecoverable();
}

int Keeper::timeout() const {
    return impl_->timeout();
}

bool Keeper::Open(const std::string& dest, int timeout) {
    return impl_->Open(dest, timeout);
}

void Keeper::Close() {
    impl_->Close();
}

void Keeper::AddAuth(const std::string& scheme, const std::string& cert,
        const AddAuthCallback& callback) {
    impl_->AddAuth(scheme, cert, callback);
}

void Keeper::Create(const std::string& path, const std::string& value,
        const std::vector<Acl>& acls, Mode::Type mode,
        const CreateCallback& callback) {
    impl_->Create(path, value, acls, mode, callback);
}

void Keeper::Delete(const std::string& path, int32_t version,
        const DeleteCallback& callback) {
    impl_->Delete(path, version, callback);
}

void Keeper::Exists(const std::string& path, 
        const NodeWatcher& watcher,
        const ExistsCallback& callback) {
    impl_->Exists(path, watcher, callback);
}

void Keeper::Get(const std::string& path,
        const NodeWatcher& watcher,
        const GetCallback& callback) {
    impl_->Get(path, watcher, callback);
}

void Keeper::Set(const std::string& path, const std::string& value,
        int32_t version, const SetCallback& callback) {
    impl_->Set(path, value, version, callback);
}

void Keeper::GetAcl(const std::string& path,
        const GetAclCallback& callback) {
    impl_->GetAcl(path, callback);
}

void Keeper::SetAcl(const std::string& path, const std::vector<Acl>& acls,
        int32_t version, const SetAclCallback& callback) {
    impl_->SetAcl(path, acls, version, callback);
}

void Keeper::GetChildren(const std::string& path,
        const ChildWatcher& watcher,
        const GetChildrenCallback& callback) {
    impl_->GetChildren(path, watcher, callback);
}

void Keeper::GetChildrenWithStat(const std::string& path,
        const ChildWatcher& watcher,
        const GetChildrenWithStatCallback& callback) {
    impl_->GetChildrenWithStat(path, watcher, callback);
}

void Keeper::Multi(const std::vector<Op*>& ops,
        const MultiCallback& callback) {
    impl_->Multi(ops, callback);
}


} // namespace cage
} // namespace ldd

