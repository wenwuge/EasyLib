// util.cc (2013-09-24)
// Li Xinjie (xjason.li@gmail.com)

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>
#include "ldd/cage/util.h"

namespace ldd {
namespace cage {

SequentialNode::SequentialNode(const std::string& prefix)
    : prefix_(prefix), sequence_(0)
{
}

bool SequentialNode::Parse(const std::string& name) {
    if (!boost::starts_with(name, prefix_)) {
        return false;
    }
    std::string idstr = name.substr(prefix_.size());
    if (idstr.empty()) {
        return false;
    }
    try {
        sequence_ = static_cast<uint32_t>(boost::lexical_cast<int32_t>(idstr));
    } catch (const boost::bad_lexical_cast&) {
        return false;
    }
    name_ = name;
    return true;
}

bool SequentialNode::operator < (const SequentialNode& rhs) const {
    CHECK(IsValid());
    CHECK(rhs.IsValid());
    CHECK_EQ(prefix(), rhs.prefix());
    return sequence() < rhs.sequence();
}

std::string GetLeaf(const std::string& path) {
    if (path.empty()) {
        return path;
    }
    std::string p = boost::trim_right_copy_if(path, boost::is_any_of("/"));
    if (path.empty()) {
        return "/";
    }
    std::string::size_type pos =  p.find_last_of('/');
    if (pos == std::string::npos) {
        return p;
    }
    return p.substr(pos + 1);
}

std::string RemoveLeaf(const std::string& path) {
    if (path.empty()) {
        return path;
    }
    std::string p = boost::trim_right_copy_if(path, boost::is_any_of("/"));
    if (path.empty()) {
        return "/";
    }
    std::string::size_type pos = p.find_last_of('/');
    if (pos == std::string::npos) {
        return "";
    }
    p = boost::trim_right_copy_if(p.substr(0, pos), boost::is_any_of("/"));
    if (p.empty()) {
        return "/";
    }
    return p;
}

} // namespace cage
} // namespace ldd
