// address.h (2013-08-17)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_ADDRESS_H_
#define LDD_NET_ADDRESS_H_

#include <netinet/in.h>
#include <string>
#include <boost/operators.hpp>

namespace ldd {
namespace net {

class Address : boost::equality_comparable<Address> {
public:
    enum Family {
        V4 = AF_INET,
        V6 = AF_INET6,
    };

    Address(Family family = V4);
    Address(const in_addr& addr);
    Address(const in6_addr& addr);
    Address(uint32_t addr); // host byte-order

    static Address FromString(const std::string& s, Family family = V4);
    static Address Any(Family family = V4);
    static Address Loopback(Family family = V4);

    Family family() const { return family_; }
    bool IsUnspecified() const;
    bool IsLoopback() const;
    bool IsV4() const { return family_ == V4; }
    bool IsV6() const { return family_ == V6; }
    std::string ToString() const;

    // V4
    const in_addr& ToV4() const;
    uint32_t ToU32() const;
    bool IsClassA() const;
    bool IsClassB() const;
    bool IsClassC() const;
    bool IsMulticast() const;

    // V6
    const in6_addr& ToV6() const;

    bool operator==(const Address& rhs) const;
private:
    union {
        in_addr v4;
        in6_addr v6;
    } addr_;
    Family family_;
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_ADDRESS_H_
