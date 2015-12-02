// cage_state.cc (2013-09-07)
// Li Xinjie (xjason.li@gmail.com)

#include "cage_state.h"

namespace ldd {
namespace cage {

const char* CageState::ToString() const {
    switch (type_) {
    case kConnected:
        return "Connected";
    case kSuspended:
        return "Suspended";
    case kReconnected:
        return "Reconnected";
    case kLost:
        return "Lost";
    };
    return "Unknown";
}

} // namespace cage
} // namespace ldd
