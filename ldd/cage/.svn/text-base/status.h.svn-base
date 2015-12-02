#ifndef LDD_CAGE_STATUS_H_
#define LDD_CAGE_STATUS_H_

#include <zookeeper/zookeeper.h>

namespace ldd {
namespace cage {

class Status {
public:
    enum Code {
        kOk = ZOK,

        kSystemError = ZSYSTEMERROR,
        kRuntimeInconsistency = ZRUNTIMEINCONSISTENCY,
        kDataInconsistency = ZDATAINCONSISTENCY,
        kConnectionLoss = ZCONNECTIONLOSS,
        kMarshallingError = ZMARSHALLINGERROR,
        kUnimplemented = ZUNIMPLEMENTED,
        kOperationTimeout = ZOPERATIONTIMEOUT,
        kBadArguments = ZBADARGUMENTS,
        kInvalidState = ZINVALIDSTATE,

        kApiError = ZAPIERROR,
        kNoNode = ZNONODE,
        kNoAuth = ZNOAUTH,
        kBadVersion = ZBADVERSION,
        kNoChildrenForEphemerals = ZNOCHILDRENFOREPHEMERALS,
        kNodeExists = ZNODEEXISTS,
        kNotEmpty = ZNOTEMPTY,
        kSessionExpired = ZSESSIONEXPIRED,
        kInvalidCallback = ZINVALIDCALLBACK,
        kInvalidACL = ZINVALIDACL,
        kAuthFailed = ZAUTHFAILED,
        kClosing = ZCLOSING,
        kNothing = ZNOTHING,
        kSessionMoved = ZSESSIONMOVED,
    };

    Status() : code_(kOk) {}
    Status(int code) : code_(code) {}
    Status& operator = (int code) { code_ = code; return *this; }

    bool operator == (const Status &rhs) { return code() == rhs.code(); }
    bool operator != (const Status &rhs) { return code() != rhs.code(); }

    int code() const { return code_; }

    bool IsOk() const { return code() == kOk; }

    bool IsSystemError() const {
        return (code() < kSystemError) && (code() > kApiError);
    }
    bool IsRuntimeInconsistency() const {return code() ==kRuntimeInconsistency;}
    bool IsDataInconsistency() const { return code() == kDataInconsistency; }
    bool IsConnectionLoss() const { return code() == kConnectionLoss; }
    bool IsMarshallingError() const { return code() == kMarshallingError; }
    bool IsUnimplemented() const { return code() == kUnimplemented; }
    bool IsOperationTimeout() const { return code() == kOperationTimeout; }
    bool IsBadArguments() const { return code() == kBadArguments; }
    bool IsInvalidState() const { return code() == kInvalidState; }
    
    bool IsApiError() const { return code() < kApiError; }
    bool IsNoNode() const { return code() == kNoNode; }
    bool IsNoAuth() const { return code() == kNoAuth; }
    bool IsBadVersion() const { return code() == kBadVersion; }
    bool IsNoChildrenForEphemerals() const {
        return code() == kNoChildrenForEphemerals; }
    bool IsNodeExists() const { return code() == kNodeExists; }
    bool IsNotEmpty() const { return code() == kNotEmpty; }
    bool IsSessionExpired() const { return code() == kSessionExpired; }
    bool IsInvalidCallback() const { return code() == kInvalidCallback; }
    bool IsInvalidACL() const { return code() == kInvalidACL; }
    bool IsAuthFailed() const { return code() == kAuthFailed; }
    bool IsClosing() const { return code() == kClosing; }
    bool IsNothing() const { return code() == kNothing; }
    bool IsSessionMoved() const { return code() == kSessionMoved; }

    const char *ToString() const { return zerror(code_); }
private:
    int code_;
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_STATUS_H_
