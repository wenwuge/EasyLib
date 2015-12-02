// outgoing_packet.cc (2013-08-26)
// Li Xinjie (xjason.li@gmail.com)

#include <glog/logging.h>
#include "ldd/net/incoming_msg.h"
#include "ldd/net/outgoing_msg.h"
#include "outgoing_packet.h"
#include "outgoing_msg_impl.h"

namespace ldd {
namespace net {

OutgoingPacket::OutgoingPacket()
    : disabled_(false)
{
    memset(header_, 0, Header::kByteSize);
}

size_t OutgoingPacket::SerializeTo(
        std::vector<ConstBuffer>* buffers) {
    if (disabled_) {
        LOG(WARNING) << "disable serialize";
        return 0U;
    }
    buffers->push_back(ConstBuffer(header_, Header::kByteSize));
    size_t size = 0;
    if (Serialize(buffers, &size)) {
        return Header::kByteSize + size;
    } else {
        LOG(WARNING) << "serialize const buffers vector failed";
        buffers->pop_back();
        return 0U;
    }
}

bool OutgoingPacketWithPayload::Serialize(
        std::vector<ConstBuffer>* buffers, size_t* size) {

    DLOG(INFO) << "enter Serialize";

    if (!InitPayload()) {
        *size = 0;
        LOG(WARNING) << "init payload failed";
        return false;
    }

    const Buffer& body = payload_.body();
    const std::map<uint8_t, Buffer>& extras = payload_.extras();

    size_t total = 0;
    size_t ext_len = 0;
    if (body.size() > 0) {
        DLOG(INFO) << "serialize body, size: " << body.size();
        buffers->push_back(ConstBuffer(body.data(), body.size()));
        total += body.size();
    }

    if (!extras.empty()) {
        size_t ext_hdr_size = ExtHeader::size(extras.size());

        DLOG(INFO) << "serialize extras, ext hdr size: " << ext_hdr_size;
        exthdr_.Reset(ext_hdr_size);

        buffers->push_back(ConstBuffer(exthdr_.data(), ext_hdr_size));
        total += ext_hdr_size;

        ExtHeader::Builder ext_hdr(exthdr_.data());
        std::map<uint8_t, Buffer>::const_iterator it = extras.begin();
        for (; it != extras.end(); ++it) {
            ext_hdr.AddExtItem(it->first, it->second.size());
            ext_len += it->second.size();
            buffers->push_back(ConstBuffer(
                        it->second.data(), it->second.size()));
        }
        total += ext_len;
        CHECK_LE(ext_len, 256*255U);
    }

    Header::Builder hdr(header_);
    hdr.set_body_size(body.size());
    hdr.set_ext_count(extras.size());
    hdr.set_ext_len(ext_len);
    hdr.Build();
    *size = total;

    Header::Parser hdr_1(header_);
    if (!hdr_1.IsValid()) {
        LOG(FATAL) << "builder header error";
        return false;
    }

    DLOG(INFO) << "Serializing " << " (id=" << hdr_1.id()
        << " body_type=" << (int)hdr_1.body_type()
        << " body_size=" << hdr_1.body_size()
        << " ext_count=" << (int)hdr_1.ext_count()
        << " ext_len=" << hdr_1.ext_len()
        << ")";

    DLOG(INFO) << "header magic: " << (int)(*buffers)[0].data()[0];

    return true;
}

OutgoingRequest::OutgoingRequest(const boost::shared_ptr<OutgoingMsg>& msg)
    : msg_(msg)
{
    Header::Builder hdr(header_);
    hdr.set_type(Header::kRequest);
    hdr.set_id(msg->id());
    hdr.set_body_type(msg->type());
}

bool OutgoingRequest::InitPayload() {
    DLOG(INFO) << "enter InitPayload";
    return msg_->impl_->OnRequest(&payload_);
}

} // namespace net
} // namespace ldd
