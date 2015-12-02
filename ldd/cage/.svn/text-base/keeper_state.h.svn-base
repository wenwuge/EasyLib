#ifndef LDD_CAGE_KEEPER_STATE_H_
#define LDD_CAGE_KEEPER_STATE_H_

#include <boost/operators.hpp>
#include <zookeeper/zookeeper.h>

namespace ldd {
namespace cage {

class KeeperState : boost::equality_comparable<KeeperState>,
            boost::equality_comparable<KeeperState, int> {
public:
    enum Type {
        kClosed = 0,
        kConnecting = 1,
        kAssociating = 2,
        kConnected = 3,
        kExpired = -112,
        kAuthFailed = -113,
    };
    KeeperState(int state) : type_(state) {}
    bool operator==(int state) const { return type_ == state; }
    bool operator==(const KeeperState &rhs) const { return type_ == rhs.type();}

    int type() const { return type_; }
    const char *ToString() const;

    bool IsClosed() const { return type_ == kClosed; }
    bool IsConnecting() const { return type_ == kConnecting; }
    bool IsAssociating() const { return type_ == kAssociating; }
    bool IsConnected() const { return type_ == kConnected; }
    bool IsExpired() const { return type_ == kExpired; }
    bool IsAuthFailed() const { return type_ == kAuthFailed; }

    bool IsUnrecoverable() const { return type_ < 0; }
private:
    int type_;
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_KEEPER_STATE_H_
