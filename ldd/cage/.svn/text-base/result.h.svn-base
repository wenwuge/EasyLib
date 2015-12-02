#ifndef LDD_CAGE_RESULTS_H_
#define LDD_CAGE_RESULTS_H_

#include <string>
#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>
#include "ldd/cage/op.h"
#include "ldd/cage/acl.h"
#include "ldd/cage/stat.h"
#include "ldd/cage/status.h"

namespace ldd {
namespace cage {

struct AddAuthResult {
    virtual ~AddAuthResult() {}
    virtual const Status& status() const = 0;
};

struct CreateResult {
    virtual ~CreateResult() {}
    virtual const Status& status() const = 0;
    virtual const std::string& path_created() const = 0;
};

struct DeleteResult {
    virtual ~DeleteResult() {}
    virtual const Status& status() const = 0;
};

struct ExistsResult {
    virtual ~ExistsResult() {}
    virtual const Status& status() const = 0;
    virtual const Stat& stat() const = 0;
};

struct GetResult {
    virtual ~GetResult() {}
    virtual const Status& status() const = 0;
    virtual const std::string& data() const = 0;
    virtual const Stat& stat() const = 0;
};

struct SetResult {
    virtual ~SetResult() {}
    virtual const Status& status() const = 0;
    virtual const Stat& stat() const = 0;
};

struct GetAclResult {
    virtual ~GetAclResult() {}
    virtual const Status& status() const = 0;
    virtual const std::vector<Acl>& acls() const = 0;
    virtual const Stat& stat() const = 0;
};

struct SetAclResult {
    virtual ~SetAclResult() {}
    virtual const Status& status() const = 0;
};

struct GetChildrenResult {
    virtual ~GetChildrenResult() {}
    virtual const Status& status() const = 0;
    virtual const std::vector<std::string>& children() const = 0;
};

struct GetChildrenWithStatResult {
    virtual ~GetChildrenWithStatResult() {}
    virtual const Status& status() const = 0;
    virtual const std::vector<std::string>& children() const = 0;
    virtual const Stat& stat() const = 0;
};

struct MultiResult {
    virtual ~MultiResult() {}
    virtual const Status& status() const = 0;
    virtual const std::vector<Op::Result*>& results() const = 0;
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_RESULTS_H_
