// outgoing_packet.h (2013-08-19)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_INTERNAL_OUTGOING_PACKET_H_
#define LDD_NET_INTERNAL_OUTGOING_PACKET_H_

#include <vector>
#include <ldd/util/const_buffer.h>
#include "protocol.h"

namespace ldd {
namespace net {

using namespace ldd::util;

class IncomingMsg;
class OutgoingMsg;

class OutgoingPacket {
public:
    OutgoingPacket();
    virtual ~OutgoingPacket() {}
    size_t SerializeTo(std::vector<ConstBuffer>* buffers);
    void Disable() { disabled_ = true; }
protected:
    virtual bool Serialize(std::vector<ConstBuffer>* buffers,
            size_t* size) {
        *size = 0;
        return true;
    }

    char header_[Header::kByteSize];
    bool disabled_;
};

class OutgoingPing : public OutgoingPacket {
public:
    OutgoingPing(MessageId id) {
        Header::Builder hdr(header_);
        hdr.set_type(Header::kPing);
        hdr.set_id(id);
        hdr.Build();
    }
};

class OutgoingPong : public OutgoingPacket {
public:
    OutgoingPong(MessageId id) {
        Header::Builder hdr(header_);
        hdr.set_type(Header::kPong);
        hdr.set_id(id);
        hdr.Build();
    }
};

class OutgoingPacketWithPayload : public OutgoingPacket {
public:
    OutgoingPacketWithPayload(const Payload& payload)
        : payload_(payload)
    {}
    OutgoingPacketWithPayload() {}

protected:
    bool Serialize(std::vector<ConstBuffer>* buffers,
            size_t* size);
    virtual bool InitPayload() = 0;
    Payload payload_;
    Buffer exthdr_;
};

class OutgoingRequest : public OutgoingPacketWithPayload {
public:
    OutgoingRequest(const boost::shared_ptr<OutgoingMsg>& msg);
    virtual bool InitPayload();
private:
    boost::shared_ptr<OutgoingMsg> msg_;
};

class OutgoingResponse: public OutgoingPacketWithPayload {
public:
    OutgoingResponse(MessageId id, const Payload& payload)
        : OutgoingPacketWithPayload(payload), last_(false), code_(0) {
        Header::Builder hdr(header_);
        hdr.set_type(Header::kResponse);
        hdr.set_id(id);
    }
    virtual bool InitPayload() {
        if (last()) {
            DLOG(INFO) << "InitPayload, type: "
                << Header::kLast
                << ", body type: " << code();
            Header::Builder hdr(header_);
            hdr.set_type(Header::kLast);
            hdr.set_body_type(code());
        }
        return true;
    }
    void set_last(bool last) { last_ = last; }
    void set_code(Code code) { code_ = code; }

    bool last() const { return last_; }
    Code code() const { return code_; }
private:
    bool last_;
    Code code_;
};

class OutgoingEnd : public OutgoingPacket {
public:
    OutgoingEnd(MessageId id, Code code) {
        Header::Builder hdr(header_);
        hdr.set_type(Header::kEnd);
        hdr.set_id(id);
        hdr.set_body_type(code);
        hdr.Build();
    }
};

class OutgoingCancel : public OutgoingPacket {
public:
    OutgoingCancel(MessageId id) {
        Header::Builder hdr(header_);
        hdr.set_type(Header::kCancel);
        hdr.set_id(id);
        hdr.Build();
    }
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_INTERNAL_OUTGOING_PACKET_H_
