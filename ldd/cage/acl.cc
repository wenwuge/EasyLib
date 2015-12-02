// acl.cc (2013-09-06)
// Li Xinjie (xjason.li@gmail.com)

#include <string.h>
#include "acl.h"

namespace ldd {
namespace cage {
    
static Id unsafe_id("world", "anyone");
static Id authed_id("auth", "");

static Acl unsafe_all_acl(Acl::kAll, Id::Unsafe());
static Acl unsafe_read_acl(Acl::kRead, Id::Unsafe());
static Acl creator_all_acl(Acl::kAll, Id::Authed());

const Id& Id::operator = (const ::Id &id) {
    if (id.scheme) {
        scheme_.assign(id.scheme);
    }
    if (id.id) {
        id_.assign(id.id);
    }
    return *this;
}
// Convertion to ::Id
Id::operator ::Id() const {
    ::Id id;
    id.scheme = strdup(scheme_.c_str());
    id.id = strdup(id_.c_str());
    return id;
}

const Id& Id::Unsafe() {
    return unsafe_id;
}
const Id& Id::Authed() {
    return authed_id;
}

const Acl& Acl::UnsafeAll() {
    return unsafe_all_acl;
}

const Acl& Acl::UnsafeRead() {
    return unsafe_read_acl;
}
const Acl& Acl::CreatorAll() {
    return creator_all_acl;
}

const Acl& Acl::operator = (const ::ACL& acl) {
    perms_ = acl.perms;
    id_ = acl.id;
    return *this;
}

Acl::operator ::ACL() const {
    ::ACL acl;
    acl.perms = perms_;
    acl.id = id_;
    return acl;
}


} // namespace cage
} // namespace ldd
