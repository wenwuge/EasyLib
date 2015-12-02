// result.h (2013-08-24)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_RESULT_H_
#define LDD_NET_RESULT_H_

#include <string>
#include "ldd/net/message.h"

namespace ldd {
namespace net {

class Result {
public:
    enum Type { kOk, kFailed, kTimedOut, kCanceled };
    static Result Ok(Code code) { return Result(kOk, code); }
    static Result Failed() { return Result(kFailed); }
    static Result TimedOut() { return Result(kTimedOut); }
    static Result Canceled() { return Result(kCanceled); }

    bool IsOk() const { return type_ == kOk; }
    bool IsFailed() const { return type_ == kFailed; }
    bool IsTimedOut() const { return type_ == kTimedOut; }
    bool IsCanceled() const { return type_ == kCanceled; }

    const std::string& ToString() const;

    Code code() const { return code_; }
private:
    Result(Type type) : type_(type), code_(0) {}
    Result(Type type, int16_t code) : type_(type), code_(code) {}

    Type type_;
    Code code_;
};


} // namespace net
} // namespace ldd

#endif // LDD_NET_RESULT_H_
