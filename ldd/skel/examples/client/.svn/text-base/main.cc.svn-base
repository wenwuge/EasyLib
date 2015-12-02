#include <gflags/gflags.h>
#include <glog/logging.h>
#include <string.h>
#include <boost/bind.hpp>

#include "ldd/net/event_loop.h"
#include "ldd/net/channel.h"
#include "ldd/net/client.h"
#include "ldd/net/outgoing_msg.h"

using namespace boost;
using namespace std;

class MyMsg : public ldd::net::TypedOutgoingMsg<2100> {
public:
    MyMsg(const std::string& s)
        : s_(s)
    {
    }
private:
    virtual bool Init(Payload* request,
            ldd::util::TimeDiff* recv_timeout,
            ldd::util::TimeDiff* done_timeout) {
        request->SetBody(ldd::net::Buffer(s_));
        *recv_timeout = ldd::util::TimeDiff::Seconds(5);
        *done_timeout = ldd::util::TimeDiff::Seconds(5);
        return true;
    }

    virtual bool Recv(const Payload& response,
            ldd::util::TimeDiff* recv_timeout) {
        LOG(INFO) << "Recv: " << response.body().ToString();
        *recv_timeout = ldd::util::TimeDiff::Seconds(5);
        return true;
    }

    virtual void Done(const Result& result) {
        if (result.IsOk()) {
            LOG(INFO) << "Ok: " << result.code();
        } else if (result.IsFailed()) {
            LOG(INFO) << result.ToString();
        } else if (result.IsTimedOut()) {
            LOG(INFO) << result.ToString();
        } else if (result.IsCanceled()) {
            LOG(INFO) << result.ToString();
        }
        channel()->event_loop()->Stop();
    }
private:
    std::string s_;
};

DEFINE_string(host, "127.0.0.1", "");
DEFINE_int32(port, 32768, "");

void OnConnected(const boost::shared_ptr<ldd::net::Channel>& c) {
}
void OnClosed(const boost::shared_ptr<ldd::net::Channel>& c) {
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, false);
    google::InitGoogleLogging(argv[0]);
    FLAGS_stderrthreshold=0; 

    ldd::net::Client::Options options;
    options.pulse_interval = 1000;
    options.notify_connected = boost::bind(&OnConnected, _1);
    options.notify_closed = boost::bind(&OnClosed, _1);
    ldd::net::Client client(options);

    ldd::net::EventLoop event_loop;
    boost::shared_ptr<ldd::net::Channel> channel = 
        client.Create(&event_loop, FLAGS_host, FLAGS_port);

    boost::shared_ptr<MyMsg> msg = boost::make_shared<MyMsg>("hahaha");
    channel->Post(msg, ldd::util::TimeDiff::Seconds(5), true);
    event_loop.Run();
}

