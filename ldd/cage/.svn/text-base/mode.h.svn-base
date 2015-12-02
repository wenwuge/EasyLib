#ifndef LDD_CAGE_MODE_H_
#define LDD_CAGE_MODE_H_

#include <zookeeper/zookeeper.h>

namespace ldd {
namespace cage {

struct Mode {
    enum Type {
        kPersistent = 0,
        kEphemeral = 1,
        kPersistentSequence = 2,
        kEphemeralSequence = 3,
    };
    static const char* ToString(Type mode);
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_MODE_H_
