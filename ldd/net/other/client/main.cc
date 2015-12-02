#include <glog/logging.h>
#include <gflags/gflags.h>
#include <ldd/net/client.h>
#include <ldd/net/channel.h>
#include <ldd/net/event_loop.h>
#include <ldd/net/endpoint.h>
#include "imsg.h"
#include "omsg.h"

DEFINE_int32(pulse, 1000, "");
DEFINE_int32(connect, 10, "");
DEFINE_int32(resolve, 10, "");
DEFINE_string(host, "localhost", "");
DEFINE_int32(port, 9230, "");
DEFINE_string(request, "This is the request", "");
DEFINE_string(extra, "This is the extra", "");
DEFINE_int32(echo, 1, "");

void NotifyConnected(const boost::shared_ptr<ldd::net::Channel>& c) {
    LOG(INFO) << "Connected " << c->peer_endpoint().ToString();
}

void NotifyConnecting(const boost::shared_ptr<ldd::net::Channel>& c) {
    LOG(INFO) << "Connecting " << c->peer_endpoint().ToString();
}

void NotifyClosed(const boost::shared_ptr<ldd::net::Channel>& c) {
    LOG(INFO) << "Closed " << c->peer_endpoint().ToString();
}

int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, false);
    google::InitGoogleLogging(argv[0]);
    FLAGS_stderrthreshold = 0;
    LOG(INFO) << "main";

    ldd::net::EventLoop event_loop;
    ldd::net::Client::Options options;
    options.pulse_interval = FLAGS_pulse;
    options.connect_timeout = FLAGS_connect;
    options.resolve_timeout = FLAGS_resolve;
    options.notify_connected = NotifyConnected;
    options.notify_connecting = NotifyConnecting;
    options.notify_closed = NotifyClosed;

    ldd::net::Client client(options);
    client.RegisterIncomingMsg<MyEchoMsg>(FLAGS_echo);
    boost::shared_ptr<ldd::net::Channel> c = client.Create(
            &event_loop, FLAGS_host, FLAGS_port);
    boost::shared_ptr<MyEmitMsg> msg =
        boost::make_shared<MyEmitMsg>(FLAGS_request, FLAGS_extra);
    c->Post(msg, ldd::util::TimeDiff::Seconds(100), true);
    event_loop.Run();
}
