#include <glog/logging.h>
#include "common.h"
#include "ldd/net/protocol.h"
#include "x_socket.h"

using namespace ldd::net;

void Get(int fd)
{
    char header[Header::kByteSize];

    Header::Builder hdr(header);
    hdr.set_type(Header::kReq);
    hdr.set_id(0);
    hdr.set_body_type(1);
    hdr.set_body_size(1);
    hdr.Build();

    int ret = x_send(fd, header, Header::kByteSize);
    if (ret == -1) {
        LOG(INFO) << "send get header failed, err=" << strerror(errno);
        return;
    }
    else if (ret == 0) {
        LOG(INFO) << "send get header failed, socket reset by server";
        return;
    }

    char body[1] = {'a'};
    ret = x_send(fd, body, 1);
    if (ret == -1) {
        LOG(INFO) << "send get body failed, err=" << strerror(errno);
        return;
    }
    else if (ret == 0) {
        LOG(INFO) << "send get body failed, socket reset by server";
        return;
    }


    ret = x_recv(fd, header, Header::kByteSize);
    if (ret == -1) {
        LOG(INFO) << "recv get header failed, err=" << strerror(errno);
        return;
    }
    else if (ret == 0) {
        LOG(INFO) << "revc get header failed, socket reset by server";
        return;
    }

    Header::Parser parser(header);
    if (!parser.IsValid()) {
        LOG(INFO) << "invalid response header";
        return;
    }

    ret = x_recv(fd, body, 1);
    if (ret == -1) {
        LOG(INFO) << "recv get body failed, err=" << strerror(errno);
        return;
    }
    else if (ret == 0) {
        LOG(INFO) << "revc get body failed, socket reset by server";
        return;
    }

    LOG(INFO) << "Get ok, result is " << body[0];

    return;
}

