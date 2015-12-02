// cage.cc (2013-09-06)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/bind.hpp>
#include <glog/logging.h>
#include "internal/cage_impl.h"

namespace ldd {
namespace cage {

Cage::Cage(net::EventLoop* event_loop, const CageListener& listener,
        RetryPolicy* policy)
    : impl_(new Impl(event_loop, listener, policy))
{
}

Cage::~Cage() {
    delete impl_;
}

bool Cage::Open(const std::string& dest, int timeout) {
    return impl_->Open(dest, timeout);
}

void Cage::Close() {
    impl_->Close();
}

void Cage::AddAuth(const std::string& scheme, const std::string& cert,
        const AddAuthCallback& callback) {
    impl_->AddAuth(scheme, cert, callback);
}

void Cage::Create(const std::string& path, const std::string& data,
        const std::vector<Acl>& acls, Mode::Type mode,
        const CreateCallback& callback) {
    impl_->Create(path, data, acls, mode, callback);
}

void Cage::Delete(const std::string& path, int32_t version,
        const DeleteCallback& callback) {
    impl_->Delete(path, version, callback);
}

void Cage::Exists(const std::string& path, const NodeWatcher& watcher,
        const ExistsCallback& callback) {
    impl_->Exists(path, watcher, callback);
}

void Cage::Get(const std::string& path, const NodeWatcher& watcher,
        const GetCallback& callback) {
    impl_->Get(path, watcher, callback);
}

void Cage::Set(const std::string& path, const std::string& data,
        int32_t version, const SetCallback& callback) {
    impl_->Set(path, data, version, callback);
}

void Cage::GetAcl(const std::string& path, const GetAclCallback& callback) {
    impl_->GetAcl(path, callback);
}

void Cage::SetAcl(const std::string& path, const std::vector<Acl>& acls,
        int32_t version, const SetAclCallback& callback) {
    impl_->SetAcl(path, acls, version, callback);
}

void Cage::GetChildren(const std::string& path, const ChildWatcher& watcher, 
        const GetChildrenCallback& callback) {
    impl_->GetChildren(path, watcher, callback);
}

void Cage::GetChildrenWithStat(const std::string& path,
        const ChildWatcher& watcher, 
        const GetChildrenWithStatCallback& callback) {
    impl_->GetChildrenWithStat(path, watcher, callback);
}

void Cage::Multi(const std::vector<Op*>& ops, const MultiCallback& callback) {
    impl_->Multi(ops, callback);
}

net::EventLoop* Cage::event_loop() const {
    return impl_->event_loop();
}

} // namespace cage
} // namespace ldd
