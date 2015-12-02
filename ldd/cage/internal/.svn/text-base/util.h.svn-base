#ifndef LDD_CAGE_INTERNAL_UTIL_H_
#define LDD_CAGE_INTERNAL_UTIL_H_

#include <string>
#include <boost/noncopyable.hpp>
#include "ldd/cage/mode.h"

namespace ldd {
namespace cage {

inline size_t MaxCreatedPathLength(const std::string& path, Mode::Type mode) {
    size_t len = path.size();
    if (mode & Mode::kPersistentSequence) {
        const size_t kSequenceSuffixLength = 12;
        len += kSequenceSuffixLength;
    }
    return len + 1;
}

void AllocateACLVector(struct ACL_vector *v, int32_t len);

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_INTERNAL_UTIL_H_
