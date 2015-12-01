// payload.cc (2013-08-16)
// Li Xinjie (xjason.li@gmail.com)

#include "payload.h"

namespace ldd {
namespace net {

#ifdef RES_COUNTER
util::Atomic<uint64_t> Payload::next_id_;
util::Atomic<uint64_t> Payload::free_id_;
#endif

Payload::Payload()
{
#ifdef RES_COUNTER
    next_id_++;
#endif
}

Payload::Payload(const Payload &load)
{
    body_ = load.body_;
    extras_ = load.extras_;

#ifdef RES_COUNTER
    next_id_++;
#endif
}

//Payload & Payload::operator=(const Payload &load)
//{
//    body_ = load.body_;
//    extras_ = load.extras_;
//    next_id_++;
//}

Payload::~Payload()
{
#ifdef RES_COUNTER
    free_id_++;
#endif
}


bool Payload::SetBody(const Buffer& buffer) {
    if (buffer.size() <= kMaxBodyLen) {
        body_ = buffer;
        return true;
    }
    return false;
}

bool Payload::SetExtra(uint8_t id, const Buffer& buffer) {
    if (buffer.size() <= kMaxExtraLen) {
        extras_[id] = buffer;
        return true;
    }
    return false;
}

void Payload::DelExtra(uint8_t id) {
    extras_.erase(id);
}

void Payload::ClearExtras() {
    extras_.clear();
}

} // namespace net
} // namespace ldd
