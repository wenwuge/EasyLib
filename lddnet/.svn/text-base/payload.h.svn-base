// payload.h (2013-08-15)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_PAYLOAD_H_
#define LDD_NET_PAYLOAD_H_

#include <stdint.h>
#include <map>
#include <ldd/net/buffer.h>

namespace ldd {
namespace net {

class Payload {
public:
    static const size_t kMaxBodyLen = (1<<24) - 1;
    static const size_t kMaxExtraLen = (1<<8) - 1;

    Payload();
    Payload(const Payload &load);
    //Payload & operator=(const Payload &load);
    
    ~Payload();
    
    const Buffer& body() const { return body_; }
    const BufferMap& extras() const { return extras_; }

    bool SetBody(const Buffer& buffer);
    bool SetExtra(uint8_t id, const Buffer& buffer);
    void DelExtra(uint8_t id);
    void ClearExtras();
private:
    Buffer body_;
    BufferMap extras_;

#ifdef RES_COUNTER
private:
    friend class Stat;
    static util::Atomic<uint64_t> next_id_;
    static util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
#endif
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_PAYLOAD_H_
