// operation.h (2013-09-07)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_CAGE_INTERNAL_OPERATION_H_
#define LDD_CAGE_INTERNAL_OPERATION_H_

#include <string>
#include <vector>
#include <boost/foreach.hpp>
#include "ldd/cage/keeper.h"

namespace ldd {
namespace cage {

template <class R>
class Operation;

template <>
class Operation<AddAuthResult> {
public:
public:
    Operation(const std::string& scheme, const std::string& cert)
        : scheme_(scheme), cert_(cert) {}
    void Do(Keeper* keeper, const AddAuthCallback& callback) {
        keeper->AddAuth(scheme_, cert_, callback);
    }
private:
    std::string scheme_;
    std::string cert_;
};

template <>
class Operation<CreateResult> {
public:
    Operation(const std::string& path, const std::string& data,
            const std::vector<Acl>& acls, Mode::Type mode)
        : path_(path), data_(data), acls_(acls), mode_(mode) {}
    void Do(Keeper* keeper, const CreateCallback& callback) {
        keeper->Create(path_, data_, acls_, mode_, callback);
    }
private:
    std::string path_;
    std::string data_;
    std::vector<Acl> acls_;
    Mode::Type mode_;
};

template <>
class Operation<DeleteResult> {
public:
    Operation(const std::string& path, int32_t version)
        : path_(path), version_(version) {}
    void Do(Keeper* keeper, const DeleteCallback& callback) {
        keeper->Delete(path_, version_, callback);
    }
private:
    std::string path_;
    int32_t version_;
};

template <>
class Operation<ExistsResult> {
public:
    Operation(const std::string& path, const NodeWatcher& watcher)
        : path_(path), watcher_(watcher) {}
    void Do(Keeper* keeper, const ExistsCallback& callback) {
        keeper->Exists(path_, watcher_, callback);
    }
private:
    std::string path_;
    NodeWatcher watcher_;
};

template <>
class Operation<GetResult> {
public:
    Operation(const std::string& path, const NodeWatcher& watcher)
        : path_(path), watcher_(watcher) {}
    void Do(Keeper* keeper, const GetCallback& callback) {
        keeper->Get(path_, watcher_, callback);
    }
private:
    std::string path_;
    NodeWatcher watcher_;
};

template <>
class Operation<SetResult> {
public:
    Operation(const std::string& path, const std::string& data, int32_t version)
        : path_(path), data_(data), version_(version) {}
    void Do(Keeper* keeper, const SetCallback& callback) {
        keeper->Set(path_, data_, version_, callback);
    }
private:
    std::string path_;
    std::string data_;
    int32_t version_;
};

template <>
class Operation<GetAclResult> {
public:
    Operation(const std::string& path) : path_(path) {}
    void Do(Keeper* keeper, const GetAclCallback& callback) {
        keeper->GetAcl(path_, callback);
    }
private:
    std::string path_;
};

template <>
class Operation<SetAclResult> {
public:
    Operation(const std::string& path, const std::vector<Acl>& acls,
            int32_t version) : path_(path), acls_(acls), version_(version) {}
    void Do(Keeper* keeper, const SetAclCallback& callback) {
        keeper->SetAcl(path_, acls_, version_, callback);
    }
private:
    std::string path_;
    std::vector<Acl> acls_;
    int32_t version_;
};

template <>
class Operation<GetChildrenResult> {
public:
    Operation(const std::string& path, const ChildWatcher& watcher)
        : path_(path), watcher_(watcher) {}
    void Do(Keeper* keeper, const GetChildrenCallback& callback) {
        keeper->GetChildren(path_, watcher_, callback);
    }
private:
    std::string path_;
    ChildWatcher watcher_;
};

template <>
class Operation<GetChildrenWithStatResult> {
public:
    Operation(const std::string& path, const ChildWatcher& watcher)
        : path_(path), watcher_(watcher) {}
    void Do(Keeper* keeper, const GetChildrenWithStatCallback& callback) {
        keeper->GetChildrenWithStat(path_, watcher_, callback);
    }
private:
    std::string path_;
    ChildWatcher watcher_;
};

template <>
class Operation<MultiResult> {
public:
    Operation(const std::vector<Op*>& ops): ops_(ops) {}
    ~Operation() {
        BOOST_FOREACH(Op* op, ops_) {
            delete op;
        }
    }
    void Do(Keeper* keeper, const MultiCallback& callback) {
        keeper->Multi(ops_, callback);
    }
private:
    std::vector<Op*> ops_;
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_INTERNAL_OPERATION_H_
