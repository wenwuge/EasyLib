// util.cc (2013-09-06)
// Li Xinjie (xjason.li@gmail.com)

#include "util.h"

namespace ldd {
namespace cage {

void AllocateACLVector(struct ACL_vector *v, int32_t len) {
    if (!len) {
        v->count = 0;
        v->data = 0;
    } else {
        v->count = len;
        v->data = (ACL*)calloc(sizeof(*v->data), len);
    }
}

} // namespace cage
} // namespace ldd
