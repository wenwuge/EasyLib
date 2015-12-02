#include <signal.h> 
#include <unistd.h>

#include <glog/logging.h>
#include <gflags/gflags.h>

#include <iostream>

#include <boost/scoped_ptr.hpp>

#include "ldd/net/event_loop.h"
#include "ldd/net/channel.h"
#include "ldd/net/client.h"

#include "ut_get.h"
#include "ut_put.h"
#include "ut_list.h"
#include "ut_incr.h"
#include "ut_append.h"
#include "ut_check.h"


using namespace ::ldd::net;

struct key_value {
    char *key;
    char *value;
};

key_value g_kv[] = {
    {"a-key", "a-value"},
    {"b-key", "b-value"},
    {"c-key", "c-value"},
    {"d-key", "d-value"},
    {"e-key", "e-value"}
};

DEFINE_string(host, "127.0.0.1", "remote host");
DEFINE_int32(port, 9257, "host port");
DEFINE_int32(threads, 1, "threads count, default: 1");

DEFINE_int32(pulse, 1000, "heartbeat interval ms, default: 1000ms");
DEFINE_string(op, "Get", "operation, Get, Put, Del");




static void sig_term(int sig){
    fprintf(stderr, "exit sig_term() ");
    exit(0);
    return;
}


//./ut_proxy -host=10.119.94.91 -logtostderr=true -op=Get


int main(int argc, char *argv[]){
    google::InitGoogleLogging(argv[0]);
    google::ParseCommandLineFlags(&argc, &argv, false);

    signal(SIGPIPE, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGALRM, SIG_IGN);
    signal(SIGCONT, SIG_IGN);
    signal(SIGTERM, sig_term);

    ldd::net::EventLoop event_loop;
    ldd::net::Client::Options options;
    options.pulse_interval = FLAGS_pulse;
    options.connect_timeout = 200;
    options.resolve_timeout = 200;
    //options.notify_connected = NotifyConnected;
    //options.notify_connecting = NotifyConnecting;
    //options.notify_closed = NotifyClosed;

    ldd::net::Client client(options);
    boost::shared_ptr<Channel> channel = client.Create(&event_loop, 
                                            FLAGS_host, FLAGS_port);


   
    if (FLAGS_op == "Put" || FLAGS_op == "Del") {
#if 0
        for (uint32_t i = 0; i < sizeof(g_kv) / sizeof(key_value); i++) {
            ldd::protocol::MutateMessage mutate;
            mutate.name_space_ = "ns1";
            mutate.mutation_.key = g_kv[i].key;
            mutate.mutation_.opt_type = FLAGS_op == "Del" ? 2 : 1;
            mutate.mutation_.value = g_kv[i].value;

            Put *put = new Put(mutate);
            put->set_conn_timeout(1000);
            put->set_send_timeout(1000);
            put->set_recv_timeout(3000);
            LOG(INFO) << "send mutate request, and sleep 200 ms";
            channel->Send(put);
            usleep(200000);
        }
#else
        ldd::protocol::MutateMessage mutate;
        mutate.name_space_ = "ns1";
        mutate.mutation_.key = "my_key";
        mutate.mutation_.opt_type = FLAGS_op == "Del" ? 2 : 1;
        mutate.mutation_.value = "my_value";
        LOG(INFO) << "send mutate request, and sleep 1 second";

        boost::shared_ptr<Put> put(new Put(mutate));
        channel->Post(put, ldd::util::TimeDiff::Milliseconds(500), true);
#endif
    }
    else if (FLAGS_op == "Get") {
        ldd::protocol::GetMessage req;
        req.name_space_ = "ns1";
        req.key_ = "my_key";

        boost::shared_ptr<Get> get(new Get(req));
        channel->Post(get, ldd::util::TimeDiff::Milliseconds(500), true);
    }
    else if (FLAGS_op == "List") {
        ldd::protocol::ListMessage req;
        req.name_space_ = "ns1";
        req.key_ = "a-key";
        req.u32limit_ = 5;
        req.u8position_ = 1;

        boost::shared_ptr<List> list(new List(req));
        channel->Post(list, ldd::util::TimeDiff::Milliseconds(500), true);
    }
    else if (FLAGS_op == "Incr") {
        ldd::protocol::AtomicIncrMessage req;
        req.name_space_ = "ns1";
        req.key_ = "int-key";
        req.s32operand_ = 10000;
        req.s32initial_ = 1;

        boost::shared_ptr<Incr> incr(new Incr(req));
        channel->Post(incr, ldd::util::TimeDiff::Milliseconds(500), true);
    }
    else if (FLAGS_op == "Append") {
        ldd::protocol::AtomicAppendMessage req;
        req.name_space_ = "ns1";
        req.key_ = "my_key";
        req.u8position_ = 2;
        req.content_ = "append";

        boost::shared_ptr<Append> append(new Append(req));
        channel->Post(append, ldd::util::TimeDiff::Milliseconds(500), true);
    }
    else if (FLAGS_op == "Check") {
        ldd::protocol::CheckMessage req;
        req.name_space_ = "ns1";
        req.key_ = "my_key";

        boost::shared_ptr<Check> check(new Check(req));
        channel->Post(check, ldd::util::TimeDiff::Milliseconds(500), true);

    }
    else {
        LOG(FATAL) << "unknow command";
    }

    event_loop.Run();
    getchar();

    return 0;
}

