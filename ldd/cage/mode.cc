#include "mode.h"

namespace ldd {
namespace cage {

const char* Mode::ToString(Type mode) {
    switch (mode) {
    case kPersistent:
        return "Persistent";
    case kEphemeral:
        return "Ephemeral";
    case kPersistentSequence:
        return "PersistentSequence";
    case kEphemeralSequence:
        return "EphemeralSequence";
    default:
        return "Unknown";
    }
}
} // namespace cage
} // namespace ldd
