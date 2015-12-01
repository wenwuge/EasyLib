#include <glog/logging.h>
#include <string.h>
#include "common.h"
#include "ldd/net/protocol.h"
#include "x_socket.h"

using namespace ldd::net;

void HeartBeat(int fd)
{
    char buf[Header::kByteSize];

    Header::Builder hdr(buf);
    hdr.set_type(Header::kReqTick);
    hdr.set_id(0);
    hdr.set_body_type(0);
    hdr.set_body_size(0);
    hdr.Build();

    int ret = x_send(fd, buf, Header::kByteSize);
    if (ret != 12) {
        LOG(INFO) << "send failed";
        return;
    }

    LOG(INFO) << "success send first packet to 9527";

    ret = x_recv(fd, buf, Header::kByteSize);
    if (ret == -1) {
        LOG(INFO) << "recv second heart failed, err=" << strerror(errno);
        return;
    }
    else if (ret == 0) {
        LOG(INFO) << "recv second heart failed, socket closed by server";
        return;
    }
    else {
        Header::Parser parser(buf);
        if (!parser.IsValid()) {
            LOG(INFO) << "first heart beat is invalid";
            return;
        }

        LOG(INFO) << "recv first heart beat success";
    }

    sleep(5);

    hdr.set_type(Header::kReqTick);
    hdr.set_id(1);
    hdr.set_body_type(0);
    hdr.set_body_size(0);
    hdr.Build();

    ret = x_send(fd, buf, Header::kByteSize);
    if (ret != Header::kByteSize) {
        LOG(INFO) << "send failed";
        return;
    }
    LOG(INFO) << "success send second packet to 9527";

    ret = x_recv(fd, buf, Header::kByteSize);
    if (ret == -1) {
        LOG(INFO) << "recv second heart failed, err=" << strerror(errno);
        return;
    }
    else if (ret == 0) {
        LOG(INFO) << "recv second heart failed, socket closed by server";
        return;
    }
    else {
        Header::Parser parser(buf);
        if (!parser.IsValid()) {
            LOG(INFO) << "second heart beat is invalid";
            return;
        }

        LOG(INFO) << "recv second heart beat success";
    }

    LOG(INFO) << "HeartBeat ok";

    return;
}

