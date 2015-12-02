#include "keeper_state.h"

namespace ldd {
namespace cage {

const char *KeeperState::ToString() const {
    switch (type_) {
    case 0:
        return "Closed";
    case kConnecting:
        return "Connecting";
    case kAssociating:
        return "Associating";
    case kConnected:
        return "Connected";
    case kExpired:
        return "Expired";
    case kAuthFailed:
        return "AuthFailed";
    }
    return "Unknown";
}

} // namespace cage
} // namespace ldd
