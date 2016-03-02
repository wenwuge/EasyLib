// error_code.cc (2013-09-24)
// Yan Gaofeng (yangaofeng@360.cn)

#include <glog/logging.h>

#include "error_code.h"

namespace lcs { namespace gateway {

const std::string ErrorCode::ToString() const 
{
    std::string e;

    switch (code_) {
        case kOk:
            e = "Ok";
            break;
        case kRequest:
            e = "invalid request, check sum, decompress, etc.";
            break;
        case kParseIni:
            e = "invalid reqeust, parse with ini failed";
            break;
        case kFieldV:
            e = "invalid request, field V format";
            break;
        case kTooManyUser:
            e = "too many user";
            break;

        case kServer:
            e = "server error";
            break;
        case kServerTimeout:
            e = "server time out";
            break;
        case kFirstLogin:
            e  = "first packet is not for login";
            break;
        case kSecondSconf:
            e  = "second packet is not for sconf";
            break;
        case kUserConnectLimit:
            e = "byond one user connections limit";
            break;
        case kUserRequestLimit:
            e = "byond one user requests in unit time";
            break;
        case kMessageType:
            e = "invalid request type";
            break;
        default:
            LOG(FATAL) << "unknown error code";
            break;
    }

    return e;
}

} /*gateway*/
} /*lcs*/

