// election.cc (2013-09-13)
// Li Xinjie (xjason.li@gmail.com)

#include <limits>
#include <memory>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <glog/logging.h>
#include "ldd/cage/recipes/election.h"

namespace ldd {
namespace cage {
namespace recipes {
using namespace std;

const std::string kElectionPrefix = "election-";

Election::Election(Cage* cage, const std::string& root,
        const Notifier& notifier)
    : cage_(cage),
      root_(root),
      notifier_(notifier),
      self_(kElectionPrefix)
{
}

void Election::Start() {
    CreateRoot();
}

void Election::CreateRoot() {
    cage_->Create(root_, "", boost::assign::list_of(Acl::UnsafeAll()),
            Mode::kPersistent, boost::bind(&Election::OnCreateRoot, this, _1));
}

void Election::OnCreateRoot(const CreateResult& result) {
    if (result.status().IsOk() || result.status().IsNodeExists()) {
        CreateSelf();
    } else {
        LOG(ERROR) << "Create election root (" << root_ << "): "
            << result.status().ToString();
        notifier_(false);
    }
}

void Election::CreateSelf() {
    cage_->Create(JoinPath(root_, kElectionPrefix), "",
            boost::assign::list_of(Acl::UnsafeAll()),
            Mode::kEphemeralSequence,
            boost::bind(&Election::OnCreateSelf, this, _1));
}

void Election::OnCreateSelf(const CreateResult& result) {
    if (result.status().IsOk()) {
        std::string self = GetLeaf(result.path_created());
        if (!self_.Parse(self)) {
            LOG(ERROR) << "Election create self with invalid result: "
                << self;
            return;
        }
        LOG(INFO) << "Election sequence = " << self_.sequence();
        CheckLeader();
    } else {
        LOG(INFO) << "Create election node failed: "
            << result.status().ToString();
        notifier_(false);
    }
}

bool Election::FindPrevNode(const std::vector<std::string>& nodes,
        std::string* prev) {
    boost::ptr_vector<SequentialNode> prev_nodes;
    BOOST_FOREACH(const std::string& node, nodes) {
        auto_ptr<SequentialNode> snode(new SequentialNode(kElectionPrefix));
        if (!snode->Parse(node)) {
            continue;
        }
        if (!(*snode < self_)) {
            continue;
        }
        prev_nodes.push_back(snode);
    }
    if (prev_nodes.empty()) {
        return false;
    }
    std::make_heap(prev_nodes.begin(), prev_nodes.end());
    *prev = prev_nodes[0].name();
    return true;
}

void Election::CheckLeader() {
    cage_->GetChildren(root_, NULL,
            boost::bind(&Election::OnGetChildren, this, _1));
}

void Election::OnGetChildren(const GetChildrenResult& result) {
    if (!result.status().IsOk()) {
        LOG(ERROR) << "Election GetChildren failed: " 
            << result.status().ToString();
        notifier_(false);
        return;
    }
    std::string prev;
    if (!FindPrevNode(result.children(), &prev)) {
        LOG(INFO) << "Election OK";
        notifier_(true);
    } else {
        cage_->Exists(JoinPath(root_, prev), 
                boost::bind(&Election::WatchPrev, this, _1),
                boost::bind(&Election::OnPrevExists, this, _1, prev));
    }
}

void Election::WatchPrev(const NodeEvent& event) {
    if (event.type() == NodeEvent::kDeleted) {
        LOG(INFO) << "Election prev node deleted, re-getchildren";
        CheckLeader();
    } else if (event.type() == NodeEvent::kChanged) {
        LOG(INFO) << "Election prev node changed, re-getchildren";
        CheckLeader();
    } else {
        LOG(INFO) << "Election prev node created, ignore";
    }
}

void Election::OnPrevExists(const ExistsResult& result,
        const std::string& prev) {
    if (result.status().IsNoNode()) {
        LOG(INFO) << "Election prev node not exists, re-getchildren";
        CheckLeader();
    } else if (result.status().IsOk()) {
        LOG(INFO) << "Election waiting prev node " << prev;
    } else {
        LOG(ERROR) << "Election check prev failed: "
            << result.status().ToString();
        notifier_(false);
    }
}

void Election::Stop() {
}

} // namespace recipes
} // namespace cage
} // namespace ldd
