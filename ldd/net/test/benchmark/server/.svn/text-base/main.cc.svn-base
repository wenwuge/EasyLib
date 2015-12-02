// main.cc (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/bind.hpp>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <ldd/net/channel.h>
#include <ldd/net/server.h>
#include <ldd/net/listener.h>
#include <ldd/net/event_loop.h>
#include <ldd/util/atomic.h>
#include <ldd/net/event.h>
#include <ldd/net/stat.h>
#include "imsg.h"
#include <signal.h>

ldd::util::Atomic<int> g_conn;
ldd::net::TimerEvent* g_timer;

DEFINE_int32(port, 9230, "");
DEFINE_int32(echo, 1, "");
DEFINE_int32(thread, 1, "");
DEFINE_int32(pulse_interval, 1000, "");
DEFINE_bool(pulse_relaxed_checking, false, "");
DEFINE_bool(pulse_lazy_emitting, false, "");

std::map<std::string, boost::shared_ptr<ldd::net::Channel> > channels;

void NotifyConnected(const boost::shared_ptr<ldd::net::Channel>& c) {
    LOG(INFO) << "Connected " << c->peer_endpoint().ToString()
        << ", fd: " << c->socket();
    g_conn++;
    
    channels.insert(std::make_pair(c->peer_endpoint().ToString(), c));
}

void NotifyClosed(const boost::shared_ptr<ldd::net::Channel>& c) {
    LOG(INFO) << "Closed " << c->peer_endpoint().ToString()
        << ", socket: " << c->socket()
        << ", map elems: " << channels.size();
    g_conn--;

    channels.erase(c->peer_endpoint().ToString());
}

void OnTimer() {
    LOG(ERROR) << "conn: " << g_conn;

    std::string stat = ldd::net::Stat::instance().Status();
    if (!stat.empty()) {
        LOG(INFO) << "lddnet status: " << stat;
    }

    g_timer->AsyncWait(OnTimer, ldd::util::TimeDiff::Seconds(1));
}

boost::ptr_vector<ldd::net::EventLoopThread> threads;

void OnSignal(int, ldd::net::EventLoop* e) {
    std::map<std::string, boost::shared_ptr<ldd::net::Channel> >::iterator c_it;
    for (c_it = channels.begin(); c_it != channels.end(); c_it++) {
        LOG(INFO) << "close channel: " << c_it->second->name();
        c_it->second->Close();
    }
    channels.clear();

    boost::ptr_vector<ldd::net::EventLoopThread>::iterator it = threads.begin();
    for (; it != threads.end(); ++it) {
        it->Stop();
    }
    e->Stop();
}

int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, false);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr=true;
    FLAGS_stderrthreshold=2;

    ldd::net::EventLoop event_loop;
    ldd::net::Listener listener(&event_loop);
    ldd::net::SignalEvent sig(&event_loop);
    sig.Add(SIGINT);
    sig.AsyncWait(
            boost::bind(&OnSignal, _1, &event_loop));

    ldd::net::Endpoint local(FLAGS_port);
    if (!listener.Open(local)) {
        LOG(ERROR) << "Listen on " << local.ToString() << " failed";
        return 1;
    }

    for (int i = 0; i < FLAGS_thread; i++) {
        ldd::net::EventLoopThread* thread = new ldd::net::EventLoopThread;
        thread->Start();
        threads.push_back(thread);
    }

    ldd::net::Server::Options options;
    options.pulse_interval = FLAGS_pulse_interval;
    options.notify_connected = NotifyConnected;
    options.notify_closed = NotifyClosed;
    options.threads = &threads;
    options.pulse_relaxed_checking = FLAGS_pulse_relaxed_checking;
    options.pulse_lazy_emitting = FLAGS_pulse_lazy_emitting;
    //options.threads_dispatching = ldd::net::Server::kHashing;
    g_timer = new ldd::net::TimerEvent(&event_loop);
    g_timer->AsyncWait(OnTimer, ldd::util::TimeDiff::Seconds(1));

    ldd::net::Server server(options);
    if (!server.Start(&listener)) {
        LOG(ERROR) << "Start on listener failed";
        return 1;
    }
    server.RegisterIncomingMsg<MyEchoMsg>();
    event_loop.Run();
    for (int i = 0; i < FLAGS_thread; i++) {
        threads[i].Join();
    }
}
