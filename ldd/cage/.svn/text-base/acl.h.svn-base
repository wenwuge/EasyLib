/*
 * =============================================================================
 *
 *       Filename:  acl.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/28/2012 17:24:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Xinjie Li (jasonli), xjason.li@gmail.com
 *        Company:  Qihoo inc.
 *
 * =============================================================================
 */

#ifndef LDD_CAGE_ACL_H_
#define LDD_CAGE_ACL_H_

#include <string>
#include <zookeeper/zookeeper.jute.h>

namespace ldd {
namespace cage {

class Id {
public:
    Id() {}
    Id(const std::string& scheme, const std::string& id)
        : scheme_(scheme), id_(id) {}
    std::string& scheme() { return scheme_; }
    std::string& id() { return id_; }

    const std::string& scheme() const { return scheme_; }
    const std::string& id() const { return id_; }

    static const Id& Unsafe();
    static const Id& Authed();

    // Convertion from ::Id
    Id(const ::Id & id) { *this = id; }
    const Id& operator = (const ::Id &id);
    // Convertion to ::Id
    operator ::Id() const;
private:
    std::string scheme_;
    std::string id_;
};

class Acl {
public:
    enum Perm {
        kRead = 1 << 0,
        kWrite = 1 << 1,
        kCreate = 1 << 2,
        kDelete = 1 << 3,
        kAdmin = 1 << 4,
        kAll = 0x1f,
    };

    Acl(): perms_(0) {}
    Acl(int perms, const Id& id): perms_(perms), id_(id) {}

    int32_t& perms() { return perms_; }
    Id& id() { return id_; }

    int32_t perms() const { return perms_; }
    const Id& id() const { return id_; }

    static const Acl& UnsafeAll();
    static const Acl& UnsafeRead();
    static const Acl& CreatorAll();

    Acl(const ::ACL& acl) { *this = acl; }
    const Acl& operator = (const ::ACL& acl);
    operator ::ACL() const;
private:
    int perms_;
    Id id_;
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_ACL_H_
