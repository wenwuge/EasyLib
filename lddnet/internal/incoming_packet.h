// incoming_packet.h (2013-08-19)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_INTERNAL_INCOMING_PACKET_H_
#define LDD_NET_INTERNAL_INCOMING_PACKET_H_

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include "protocol.h"
#include "channel_impl.h"

namespace ldd {
namespace net {

//class PacketReader;

class IncomingPacket : public boost::enable_shared_from_this<IncomingPacket> {
public:
    IncomingPacket(Channel::Impl* channel)
        : channel_(channel), channel_owner_(channel->owner())
    {}
    virtual ~IncomingPacket();

    virtual void Process() = 0;
    Channel::Impl* channel() const { return channel_; }
    Header::Parser header() const { return channel_->header(); }
private:
    Channel::Impl* channel_;
    boost::shared_ptr<Channel> channel_owner_;
};

class IncomingPacketWithPayload : public IncomingPacket {
public:
    IncomingPacketWithPayload(Channel::Impl* channel
            , const char* name
            , const boost::shared_ptr<Buffer> &packet)
        : IncomingPacket(channel), name_(name), packet_(packet) {}

    virtual void Process();
    boost::shared_ptr<Buffer> packet() { return packet_; }

protected:
    void ProcessPacket();

    virtual void Done() = 0;

    const char* name() const { return name_; }
    const Payload& payload() const { return payload_; }
private:
    const char* name_;
    boost::shared_ptr<Buffer> packet_;
    Payload payload_;
};

class IncomingPing: public IncomingPacket {
public:
    IncomingPing(Channel::Impl* channel)
        : IncomingPacket(channel) {}
    virtual void Process();
};

class IncomingPong: public IncomingPacket {
public:
    IncomingPong(Channel::Impl* channel)
        : IncomingPacket(channel) {}
    virtual void Process();
};

class IncomingRequest: public IncomingPacketWithPayload {
public:
    IncomingRequest(Channel::Impl* channel
            , const boost::shared_ptr<Buffer> &packet)
        : IncomingPacketWithPayload(channel, "request", packet) {}
private:
    virtual void Done();
};

class IncomingResponse: public IncomingPacketWithPayload {
public:
    IncomingResponse(Channel::Impl* channel, const boost::shared_ptr<Buffer> &packet)
        : IncomingPacketWithPayload(channel,
                channel->header().type() == Header::kLast? "last" : "response",
                packet)
        {}
private:
    virtual void Done();
};

class IncomingEnd: public IncomingPacket {
public:
    IncomingEnd(Channel::Impl* channel)
        : IncomingPacket(channel) {}
    virtual void Process();
};

class IncomingCancel: public IncomingPacket {
public:
    IncomingCancel(Channel::Impl* channel)
        : IncomingPacket(channel) {}
    virtual void Process();
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_INTERNAL_INCOMING_PACKET_H_
