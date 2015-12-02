// test.cc (2013-09-06)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/bind.hpp>
#include <ldd/cage/cage.h>
#include <ldd/net/event_loop.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <ldd/cage/retry_policy.h>
#include <ldd/cage/recipes/election.h>

using namespace std;

DEFINE_string(zk, "localhost:2181", "");
DEFINE_int32(timeout, 5000, "");

ldd::cage::Cage* cage = NULL;
ldd::cage::recipes::Election* election = NULL;
ldd::net::EventLoop* event_loop;

void WatchNode(const ldd::cage::NodeEvent& event) {
    if (event.IsCreated()) {
        LOG(INFO) << event.path() << " created";
    } else if (event.IsDeleted()) {
        LOG(INFO) << event.path() << " deleted";
    } else if (event.IsChanged()) {
        LOG(INFO) << event.path() << " changed";
    }
}

void OnDelete(const ldd::cage::DeleteResult& result) {
    if (result.status().IsOk()) {
        LOG(INFO) << "Delete Ok";
    } else if (result.status().IsNoNode()) {
        LOG(WARNING) << "Node doesn't exists";
    } else if (result.status().IsNotEmpty()) {
        LOG(ERROR) << "Node isn't empty";
    } else {
        LOG(ERROR) << "Delete error: " << result.status().ToString();
    }
}

void OnGet(const ldd::cage::GetResult& result) {
    if (result.status().IsOk()) {
        LOG(INFO) << "Get Ok, data = " << result.data();
        cage->Delete("/cage", -1, OnDelete);
    } else if (result.status().IsNoNode()) {
        LOG(WARNING) << "Node doesn't exists";
    } else {
        LOG(ERROR) << "Get error: " << result.status().ToString();
    }
}

void OnCreated(const ldd::cage::CreateResult& result,
        ldd::cage::Cage* cage) {
    if (result.status().IsOk()) {
        LOG(INFO) << "Create ok: " << result.path_created();
        cage->Get("/cage", WatchNode, OnGet);
    } else if (result.status().IsNoNode()) {
        LOG(ERROR) << "Create failed: parent not exists";
    } else if (result.status().IsNodeExists()) {
        LOG(ERROR) << "Create failed: already exists";
    } else if (result.status().IsNoChildrenForEphemerals()) {
        LOG(ERROR) << "Create failed: parent is ephemeral node";
    } else {
        LOG(ERROR) << "Create error: " << result.status().ToString();
    }
}

void ElectionCallback(bool ok) {
    LOG(INFO) << "Election " << (ok ? "OK" : "Failed");
    //cage->Close();
}

void NotifyCage(const ldd::cage::CageState& state){
    LOG(INFO) << "Notified: " << state.ToString();
    if (state.IsConnected()) {
        election = new ldd::cage::recipes::Election(cage, "/election",
                ElectionCallback);
        election->Start();

        vector<ldd::cage::Acl> acls;
        acls.push_back(ldd::cage::Acl::UnsafeAll());
        cage->Create("/cage", "haha", acls, ldd::cage::Mode::kEphemeral,
                boost::bind(&OnCreated, _1, cage));
    } else if (state.IsSuspended()) {
    } else if (state.IsReconnected()) {
    } else if (state.IsLost()) {
        delete cage;
        cage = new ldd::cage::Cage(event_loop, boost::bind(&NotifyCage, _1),
                ldd::cage::RetryPolicy::NewRetryNTimesPolicy(
                    3, ldd::util::TimeDiff::Seconds(1)));
        if (!cage->Open(FLAGS_zk, FLAGS_timeout)) {
            exit(1);
        }
    } else {
        LOG(FATAL) << "unknown state";
    }
}

int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, false);
    google::InitGoogleLogging(argv[0]);
    FLAGS_stderrthreshold = 0;
    event_loop = new ldd::net::EventLoop();

    cage = new ldd::cage::Cage(event_loop, boost::bind(&NotifyCage, _1),
            ldd::cage::RetryPolicy::NewRetryNTimesPolicy(
                3, ldd::util::TimeDiff::Seconds(1)));
    if (!cage->Open(FLAGS_zk, FLAGS_timeout)) {
        return 1;
    }
    LOG(INFO) << "Init ok";
    event_loop->Run();
    delete event_loop;
}
