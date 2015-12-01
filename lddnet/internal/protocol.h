#ifndef LDD_NET_PROTOCOL_H_
#define LDD_NET_PROTOCOL_H_

#include <stdint.h>

namespace ldd {
namespace net {

/**
 * Header format (all in big-endian):
 *
 * | magic  |  type  |              msg-id               |    body-type    |
 * |--------|--------|--------|--------|--------|--------|--------|--------|
 *
 * |         body-length      | ext-n  |     ext-len     |    checksum     |
 * |--------|--------|--------|--------|--------|--------|--------|--------|
 *
 *
 *
 */
class Header {
public:
    const static size_t kByteSize = 16;
    const static uint8_t kMagic = 0xA1;

    enum Type {
        // Uplink type
        kRequest = 0,
        kPing,
        kCancel,

        // Downlink type
        kResponse = 128,
        kPong,
        kLast,
        kEnd,
    };

    class Parser {
    public:
        Parser(const char* buf);
        bool IsValid() const;

        uint8_t type() const;
        uint32_t id() const;
        uint16_t body_type() const;
        uint32_t body_size() const;
        uint8_t ext_count() const;
        uint16_t ext_len() const;
    private:
        const char* buf_;
    };

    class Builder {
    public:
        Builder(char* buf);
        void Build();
        
        void set_type(uint8_t type);
        void set_id(uint32_t id);
        void set_body_type(uint16_t body_type);
        void set_body_size(uint32_t body_size);
        void set_ext_count(uint8_t ext_count);
        void set_ext_len(uint16_t ext_len);
    private:
        char* buf_;
    };
};

class ExtHeader {
public:
    static const int kUnitSize = 2;
    static uint32_t size(int count) { return count * 2; }

    class Parser {
    public:
        Parser(uint8_t ext_count, const char* buf);
        bool IsValid(uint16_t ext_len) const;

        bool GetExtItem(int index, uint8_t *type, uint8_t *len);
    private:
        uint8_t ext_count_;
        const char* ext_buf_;
    };

    class Builder {
    public:
        Builder(char* buf);
        void AddExtItem(uint8_t type, uint8_t len);
    private:
        int ext_count_;
        char* ext_buf_;
    };
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_PROTOCOL_H_

