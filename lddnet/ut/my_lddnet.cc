// my_lddnet.cc (2014-07-17)
// Yan Gaofeng (yangaofeng@360.cn)

#include <glog/logging.h>
#include <ldd/net/channel.h>
#include <ldd/net/endpoint.h>

#include "imsg.h"
#include "omsg.h"
#include "my_lddnet.h"

const std::string REQUEST = "This is the request";
const std::string EXTRAS = "This is the extra";

const std::string SERVER_HOST = "localhost";
const int SERVER_PORT = 60000;

const int PULSE = 1000;
const int CONNECT = 10;
const int RESOLVE = 10;


void NotifyConnected(const boost::shared_ptr<ldd::net::Channel>& c) {
    LOG(INFO) << "Connected " << c->peer_endpoint().ToString();
}

void NotifyConnecting(const boost::shared_ptr<ldd::net::Channel>& c) {
    LOG(INFO) << "Connecting " << c->peer_endpoint().ToString();
}

void NotifyClosed(const boost::shared_ptr<ldd::net::Channel>& c) {
    LOG(INFO) << "Closed " << c->peer_endpoint().ToString();
}

bool MyLddNet::Init()
{
    //init server
    ldd::net::Endpoint local(SERVER_PORT);
    if (!listener_.Open(local)) {
        LOG(ERROR) << "Listen on " << local.ToString() << " failed";
        return false;
    }

    ldd::net::Server::Options so;
    so.pulse_interval = 1000;
    so.notify_connected = NotifyConnected;
    so.notify_closed = NotifyClosed;

    server_.reset(new ldd::net::Server(so));
    if (!server_->Start(&listener_)) {
        LOG(ERROR) << "Start on listener failed";
        return false;
    }

    server_->RegisterIncomingMsg<MyEchoMsg>(this);

    //init client
    ldd::net::Client::Options options;
    options.pulse_interval = PULSE;
    options.connect_timeout = CONNECT;
    options.resolve_timeout = RESOLVE;
    options.notify_connected = NotifyConnected;
    options.notify_connecting = NotifyConnecting;
    options.notify_closed = NotifyClosed;

    //create client
    client_.reset(new ldd::net::Client(options));
    client_->RegisterIncomingMsg<MyEchoMsg>(this);

    return true;
}

void MyLddNet::Finalize()
{
    LOG(INFO) << "Enter Finalize";
}

bool MyLddNet::Echo()
{
    //connect to server
    boost::shared_ptr<ldd::net::Channel> c = client_->Create(
            &loop_, SERVER_HOST, SERVER_PORT);

    //send msg
    boost::shared_ptr<MyEmitMsg> msg =
        boost::make_shared<MyEmitMsg>(this, REQUEST, EXTRAS);
    c->Post(msg, ldd::util::TimeDiff::Seconds(100), true);

    loop_.Run();

    return ok_;
}
