// endpoint.h (2013-08-17)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_ENDPOING_H_
#define LDD_NET_ENDPOING_H_

#include "ldd/net/address.h"

namespace ldd {
namespace net {

class Endpoint {
public:
    Endpoint(): address_(Address::V4), port_(0) {}
    Endpoint(uint16_t port): address_(Address::V4), port_(port) {}
    Endpoint(Address::Family family, uint16_t port)
        : address_(family), port_(port) {}
    Endpoint(const Address& address, uint16_t port)
        : address_(address), port_(port) {}

    const Address& address() const { return address_; }
    uint16_t port() const { return port_; }

    void set_port(uint16_t port) { port_ = port; }
    void set_address(const Address& address) { address_ = address; }

    std::string ToString() const;
private:
    Address address_;
    uint16_t port_;
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_ENDPOING_H_
