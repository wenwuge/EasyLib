#include <zookeeper/zookeeper.jute.h>
#include "op.h"
#include "internal/util.h"
#include "internal/op_result.h"

namespace ldd {
namespace cage {

OpCreate::OpCreate(const std::string& path, const std::string& data,
        const std::vector<Acl>& acls, Mode::Type mode)
    : Op(path)
    , data_(data)
    , mode_(mode)
{
    AllocateACLVector(&acls_, (int32_t)acls.size());
    std::copy(acls.begin(), acls.end(), acls_.data);
}

OpCreate::~OpCreate() {
    ::deallocate_ACL_vector(&acls_);
}

Op::Result* OpCreate::MakeResult(zoo_op_t* zop) const {
    OpCreateResult* result = new OpCreateResult(path_, mode_);
    zoo_create_op_init(zop, path_.c_str(), data_.data(), data_.size(),
            &acls_, mode_, result->buffer(), result->buflen());
    return result;
}

OpDelete::OpDelete(const std::string& path, int32_t version)
    : Op(path)
    , version_(version)
{
}

Op::Result* OpDelete::MakeResult(zoo_op_t* zop) const {
    OpDeleteResult* result = new OpDeleteResult();
    zoo_delete_op_init(zop, path_.c_str(), version_);
    return result;
}

OpSet::OpSet(const std::string& path, const std::string& data, int32_t version)
    : Op(path)
    , data_(data)
    , version_(version)
{
}

Op::Result* OpSet::MakeResult(zoo_op_t* zop) const {
    OpSetResult* result = new OpSetResult();
    zoo_set_op_init(zop, path_.c_str(), data_.data(), data_.size(),
            version_, result->buffer());
    return result;
}

OpCheck::OpCheck(const std::string& path, int32_t version)
    : Op(path)
    , version_(version)
{
}

Op::Result* OpCheck::MakeResult(zoo_op_t* zop) const {
    OpCheckResult* result = new OpCheckResult();
    zoo_check_op_init(zop, path_.c_str(), version_);
    return result;
}

} // namespace cage
} // namespace ldd

