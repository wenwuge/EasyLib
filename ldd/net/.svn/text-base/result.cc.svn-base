// result.cc (2013-08-27)
// Li Xinjie (xjason.li@gmail.com)

#include <glog/logging.h>
#include "result.h"

namespace ldd {
namespace net {

const std::string& Result::ToString() const {
    switch (type_) {
    case kOk: {
        static std::string s("Ok");
        return s;
    }
    case kFailed: {
        static std::string s("Failed");
        return s;
    }
    case kTimedOut: {
        static std::string s("TimedOut");
        return s;
    }
    case kCanceled: {
        static std::string s("Canceled");
        return s;
    }
    default:
        LOG(FATAL) << "Invalid result type";
    }
    static std::string s("Unknown");
    return s;
}

} // namespace net
} // namespace ldd
