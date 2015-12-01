// endpoint.cc (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#include <sstream>
#include "ldd/net/endpoint.h"

namespace ldd {
namespace net {

std::string Endpoint::ToString() const {
    std::stringstream ss;
    ss << address_.ToString() << ":" << port_;
    return ss.str();
}

} // namespace net
} // namespace ldd
