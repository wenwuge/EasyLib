// cage_state.h (2013-09-07)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_CAGE_CAGE_STATE_H_
#define LDD_CAGE_CAGE_STATE_H_

#include <boost/operators.hpp>

namespace ldd {
namespace cage {

class CageState : boost::equality_comparable<CageState>,
                boost::equality_comparable<CageState, int> {
public:
    enum Type {
        kConnected = 1,
        kSuspended = 2,
        kReconnected = 3,
        kLost = 4,
    };
    CageState() : type_(0) {}
    CageState(Type type): type_(type) {}
    static CageState Connected() { return CageState(kConnected); }
    static CageState Suspended() { return CageState(kSuspended); }
    static CageState Reconnected() { return CageState(kReconnected); }
    static CageState Lost() { return CageState(kLost); }
    bool operator==(const CageState& rhs) const { return type_ == rhs.type_; }
    bool operator==(int state) const { return type_ == state; }
    int type() const { return type_; }
    const char* ToString() const;

    bool IsConnected() const { return type_ == kConnected; }
    bool IsSuspended() const { return type_ == kSuspended; }
    bool IsReconnected() const { return type_ == kReconnected; }
    bool IsLost() const { return type_ == kLost; }
    bool IsUnknown() const { return type_ < kConnected || type_ > kLost; }
private:
    int type_;
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_CAGE_STATE_H_
