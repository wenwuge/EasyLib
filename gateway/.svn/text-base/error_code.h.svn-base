// error_code.h (2013-09-24)
// Yan Gaofeng (yangaofeng@360.cn)

#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__

#include <string>
#include <stdint.h>

namespace lcs { namespace gateway {

typedef uint32_t Code;

class ErrorCode {
    enum {
        //for server
        kOk = 0,
        kRequest,
        kParseIni,
        kFieldV,
        kTooManyUser,

        //for gateway
        kServer,
        kServerTimeout,
        kFirstLogin,
        kSecondSconf,
        kUserConnectLimit,
        kUserRequestLimit,
        kMessageType
    };

public:
    static ErrorCode Ok() { return ErrorCode(); }
    static ErrorCode Request() { return ErrorCode(kRequest); }
    static ErrorCode ParseIni() { return ErrorCode(kParseIni); }
    static ErrorCode FieldV() { return ErrorCode(kFieldV); }
    static ErrorCode TooManyUser() { return ErrorCode(kTooManyUser); }

    static ErrorCode Server() { return ErrorCode(kServer); }
    static ErrorCode ServerTimeout() { return ErrorCode(kServerTimeout); }
    static ErrorCode FirstLogin() { return ErrorCode(kFirstLogin); }
    static ErrorCode SecondSconf() { return ErrorCode(kSecondSconf); }
    static ErrorCode UserConnectLimit() { return ErrorCode(kUserConnectLimit); }
    static ErrorCode UserRequestLimit() { return ErrorCode(kUserRequestLimit); }
    static ErrorCode MessageType() { return ErrorCode(kMessageType); }

    bool IsOk() { return code_ == kOk; }
    bool IsReqeust() { return code_ == kRequest; }
    bool IsParseIni() { return kParseIni == code_; }
    bool IsFieldV() { return kFieldV == code_; }
    bool IsTooManyUser() { return kTooManyUser == code_; }

    bool IsServer() { return kServer == code_; }
    bool IsServerTimerout() { return kServerTimeout == code_; }
    bool IsFirstLogin() { return kFirstLogin == code_; }
    bool IsSecondSconf() { return kSecondSconf == code_; }
    bool IsUserConnectLimit() { return kUserConnectLimit == code_; }
    bool IsUserRequestLimit() { return kUserRequestLimit == code_; }
    bool IsMessageType() { return kMessageType == code_; }

    Code code() { return code_; }
    const std::string ToString() const;

private:
    ErrorCode() : code_(ErrorCode::kOk) {}
    ErrorCode(Code code) : code_(code) {}

private:
    Code code_;
};


} /*gateway*/
} /*lcs*/


#endif /*__ERROR_CODE_H__*/

