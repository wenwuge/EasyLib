// incoming_packet.cc (2013-08-19)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/bind.hpp>
#include <glog/logging.h>
#include "incoming_packet.h"
#include "incoming_msg_impl.h"
#include "outgoing_msg_impl.h"

namespace ldd {
namespace net {

IncomingPacket::~IncomingPacket() {
}

void IncomingPing::Process() {
    channel()->OnPing();
}

void IncomingPong::Process() {
    channel()->OnPong();
}

void IncomingPacketWithPayload::Process() {
    ProcessPacket();
}

void IncomingPacketWithPayload::ProcessPacket() {

    if (!packet_) {
        //note, may be kLast, body is emtpy
        DLOG(INFO) << "receive response without payload, type: " 
            << (int)header().type();
        Done();
        return;
    }

    //get body from packet
    char *p = packet_->data();

    //process body
    if (header().body_size() > 0) {
        Buffer body(p, header().body_size());
        payload_.SetBody(body);
        p += header().body_size();
    }

    if (header().ext_count() > 0) {

        //process extra header
        char *ext_header = p;
        p += ExtHeader::size(header().ext_count());

        ExtHeader::Parser exthdr(header().ext_count(), ext_header);
        if (!exthdr.IsValid(header().ext_len())) {
            LOG(ERROR) << name()
                << "Parse " << name() << " (id=" << (int)header().id()
                << ") extra header failed";
            channel()->OnError();
            return;
        }

        int ext_len = 0;
        for (int i = 0; i < header().ext_count(); ++i) {
            uint8_t type, len;
            exthdr.GetExtItem(i, &type, &len);
            ext_len += len;
        }

        //check ext len
        if (ext_len != header().ext_len()) {
            LOG(ERROR) << "invalid ext header len";
            channel()->OnError();
            return;
        }

        //process ext body
        for (int i = 0; i < header().ext_count(); ++i) {
            uint8_t type, len;
            exthdr.GetExtItem(i, &type, &len);

            Buffer ext(p, len);
            p += len;

            payload_.SetExtra(type, ext);
        }
    }

    DLOG(INFO) << channel()->name()
        << "Reading "<< name() << " (id=" << header().id()
        << " body_type=" << (int)header().body_type()
        << " body_size=" << header().body_size()
        << " ext_count=" << (int)header().ext_count()
        << " ext_len=" << header().ext_len()
        << ")";

    Done();
}

void IncomingRequest::Done()
{
    boost::shared_ptr<IncomingMsg> msg =
        channel()->registry()->NewIncomingMsg(header().body_type());
    if (!msg) {
        DLOG(INFO) << "no valid register incoming msg, try to create dyanmic incoming";
        boost::shared_ptr<DynamicIncomingMsg> dynamic_msg =
            channel()->registry()->NewDefaultIncomingMsg();
        if (!dynamic_msg) {
            LOG(ERROR) << channel()->name()
                << "Request (id=" << (int)header().id()
                << ") has unkown body_type = " << header().body_type();
            channel()->OnError();
            return;
        }
        dynamic_msg->set_type(header().body_type());
        msg = dynamic_msg;
    }

    DLOG(INFO) << "call msg impl Init method";
    msg->impl_->Init(header().id(), channel()->owner());
    if (!channel()->AddIncomingMsg(msg)) {
        LOG(ERROR) << channel()->name()
            << "Incoming "<< name() << " (id=" << (int)header().id()
            << ") is duplicated with existing message";
        channel()->OnError();
        return;
    }

    DLOG(INFO) << "call channel OnIncomingPacket()";
    channel()->OnIncomingPacket();

    DLOG(INFO) << "call channel impl OnReqeust method";
    msg->impl_->OnRequest(payload());
}

void IncomingResponse::Done() {
    bool last = header().type() == Header::kLast;
    boost::shared_ptr<OutgoingMsg> msg = channel()->GetOutgoingMsg(header().id(), last);
    if (!msg) {
        LOG(ERROR) << channel()->name()
            << "Incoming " << name() << " (id=" << (int)header().id()
            << ") can't be found in outgoing msg, ignored";
        return;
    }
    channel()->OnIncomingPacket();
    // In Last response, body_type field is representing return code.
    msg->impl_->OnResponse(payload(), last, header().body_type());
}

void IncomingEnd::Process() {
    boost::shared_ptr<OutgoingMsg> msg =
        channel()->GetOutgoingMsg(header().id(), true);
    if (!msg) {
        LOG(ERROR) << channel()->name()
            << "Incoming end (id=" << (int)header().id()
            << ") can't be found in outgoing msg, ignored";
        return;
    }
    channel()->OnIncomingPacket();
    // In End response, body_type field is representing return code.
    msg->impl_->OnOk(header().body_type());
}

void IncomingCancel::Process() {
    boost::shared_ptr<IncomingMsg> msg =
        channel()->GetIncomingMsg(header().id(), true);
    if (!msg) {
        LOG(ERROR) << channel()->name()
            << "Incoming cancel (id=" << (int)header().id()
            << ") can't be found in incoming msg, ignored";
        return;
    }
    channel()->OnIncomingPacket();
    LOG(INFO) << channel()->name()
        << "Canceling incoming msg (id=" << (int)header().id() << ")";
    msg->impl_->DoCancel();
}

} // namespace net
} // namespace ldd
