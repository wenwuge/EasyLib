// address.cc (2013-08-17)
// Li Xinjie (xjason.li@gmail.com)

#include <arpa/inet.h>
#include <glog/logging.h>
#include "address.h"

namespace ldd {
namespace net {

Address::Address(Family family)
    : family_(family)
{
    CHECK(family == V4 || family == V6);
    memset(&addr_, 0, sizeof(addr_));
}

Address::Address(const in_addr& addr)
    : family_(V4)
{
    addr_.v4.s_addr = addr.s_addr;
}

Address::Address(uint32_t addr)
    : family_(V4)
{
    addr_.v4.s_addr = htonl(addr);
}

Address::Address(const in6_addr& addr)
    : family_(V6)
{
    memcpy(&addr_.v6, &addr, sizeof(in6_addr));
}

const in_addr& Address::ToV4() const {
    CHECK_EQ(family_, V4);
    return addr_.v4;
}

const in6_addr& Address::ToV6() const {
    CHECK_EQ(family_, V6);
    return addr_.v6;
}

uint32_t Address::ToU32() const {
    CHECK_EQ(family_, V4);
    return ntohl(addr_.v4.s_addr);
}

Address Address::FromString(const std::string& s, Family family) {
    CHECK(family == V4 || family == V6);
    Address temp;
    inet_pton(family, s.c_str(), &temp.addr_);
    return temp;
}

Address Address::Any(Family family) {
    return Address(family);
}

Address Address::Loopback(Family family) {
    if (family == V4) {
        return Address(0x7F000001);
    } else if (family == V6) {
        Address tmp(V6);
        tmp.addr_.v6.s6_addr[15] = 1;
        return tmp;
    } else {
        LOG(FATAL) << "Invalid family " << family;
    }
}

std::string Address::ToString() const {
    if (family() == V4) {
        char addr_str[INET_ADDRSTRLEN];
        const char* addr =
            inet_ntop(family(), &addr_, addr_str, INET_ADDRSTRLEN);
        if (!addr) {
            return std::string();
        }
        return addr_str;
    } else {
        char addr_str[INET6_ADDRSTRLEN];
        const char* addr =
            inet_ntop(family(), &addr_, addr_str, INET6_ADDRSTRLEN);
        if (!addr) {
            return std::string();
        }
        return addr_str;
    }
}

bool Address::IsClassA() const {
    CHECK_EQ(family_, V4);
    return (ToU32() & 0x80000000) == 0;
}

bool Address::IsClassB() const {
    CHECK_EQ(family_, V4);
    return (ToU32() & 0xC0000000) == 0x80000000;
}
bool Address::IsClassC() const {
    CHECK_EQ(family_, V4);
    return (ToU32() & 0xE0000000) == 0xC0000000;
}
bool Address::IsMulticast() const {
    CHECK_EQ(family_, V4);
    return (ToU32() & 0xF0000000) == 0xE0000000;
}

bool Address::IsUnspecified() const {
    if (family_ == V4) {
        return addr_.v4.s_addr == 0;
    } else {
        return ((addr_.v6.s6_addr[0] == 0) && (addr_.v6.s6_addr[1] == 0)
            &&  (addr_.v6.s6_addr[2] == 0) && (addr_.v6.s6_addr[3] == 0)
            &&  (addr_.v6.s6_addr[4] == 0) && (addr_.v6.s6_addr[5] == 0)
            &&  (addr_.v6.s6_addr[6] == 0) && (addr_.v6.s6_addr[7] == 0)
            &&  (addr_.v6.s6_addr[8] == 0) && (addr_.v6.s6_addr[9] == 0)
            &&  (addr_.v6.s6_addr[10] == 0) && (addr_.v6.s6_addr[11] == 0)
            &&  (addr_.v6.s6_addr[12] == 0) && (addr_.v6.s6_addr[13] == 0)
            &&  (addr_.v6.s6_addr[14] == 0) && (addr_.v6.s6_addr[15] == 0));
    }
}

bool Address::IsLoopback() const {
    if (family_ == V4) { 
        return (ToU32() & 0xFF000000) == 0x7F000000;
    } else {
        return ((addr_.v6.s6_addr[0] == 0) && (addr_.v6.s6_addr[1] == 0)
            && (addr_.v6.s6_addr[2] == 0) && (addr_.v6.s6_addr[3] == 0)
            && (addr_.v6.s6_addr[4] == 0) && (addr_.v6.s6_addr[5] == 0)
            && (addr_.v6.s6_addr[6] == 0) && (addr_.v6.s6_addr[7] == 0)
            && (addr_.v6.s6_addr[8] == 0) && (addr_.v6.s6_addr[9] == 0)
            && (addr_.v6.s6_addr[10] == 0) && (addr_.v6.s6_addr[11] == 0)
            && (addr_.v6.s6_addr[12] == 0) && (addr_.v6.s6_addr[13] == 0)
            && (addr_.v6.s6_addr[14] == 0) && (addr_.v6.s6_addr[15] == 1));
    }
}

bool Address::operator==(const Address& rhs) const {
    if (family_ != rhs.family_) {
        return false;
    }
    if (family_ == V4) {
        return (memcmp(&addr_, &rhs.addr_, sizeof(addr_.v4)) == 0);
    } else {
        return (memcmp(&addr_, &rhs.addr_, sizeof(addr_.v6)) == 0);
    }
}

} // namespace net
} // namespace ldd
