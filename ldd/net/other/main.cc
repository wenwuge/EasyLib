// main.cc (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <ldd/net/channel.h>
#include <ldd/net/server.h>
#include <ldd/net/listener.h>
#include <ldd/net/event_loop.h>
#include "imsg.h"
#include "omsg.h"

DEFINE_int32(port, 9230, "");
DEFINE_int32(echo, 1, "");

void NotifyConnected(const boost::shared_ptr<ldd::net::Channel>& c) {
    LOG(INFO) << "Connected " << c->peer_endpoint().ToString();
}

void NotifyClosed(const boost::shared_ptr<ldd::net::Channel>& c) {
    LOG(INFO) << "Closed " << c->peer_endpoint().ToString();
}

int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, false);
    google::InitGoogleLogging(argv[0]);
    FLAGS_stderrthreshold = 0;

    ldd::net::EventLoop event_loop;
    ldd::net::Listener listener(&event_loop);

    ldd::net::Endpoint local(FLAGS_port);
    if (!listener.Open(local)) {
        LOG(ERROR) << "Listen on " << local.ToString() << " failed";
        return 1;
    }
    ldd::net::Server::Options options;
    options.pulse_interval = 1000;
    options.notify_connected = NotifyConnected;
    options.notify_closed = NotifyClosed;

    ldd::net::Server server(options);
    if (!server.Start(&listener)) {
        LOG(ERROR) << "Start on listener failed";
        return 1;
    }
    server.RegisterIncomingMsg<MyEchoMsg>(FLAGS_echo);
    event_loop.Run();
}
