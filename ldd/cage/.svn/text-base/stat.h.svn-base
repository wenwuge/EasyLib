#ifndef LDD_CAGE_STAT_H_
#define LDD_CAGE_STAT_H_

#include <zookeeper/zookeeper.jute.h>

namespace ldd {
namespace cage {

struct Stat : public ::Stat {
    Stat() { Clear(); }
    ~Stat() {}

    void Clear() {
        czxid = 0;
        mzxid = 0;
        ctime = 0;
        mtime = 0;
        version = 0;
        cversion = 0;
        aversion = 0;
        ephemeralOwner = 0;
        dataLength = 0;
        numChildren = 0;
        pzxid = 0;
    }

    Stat(const ::Stat &stat) {
        *this = stat;
    }

    const Stat& operator = (const ::Stat &stat) {
        czxid = stat.czxid;
        mzxid = stat.mzxid;
        ctime = stat.ctime;
        mtime = stat.mtime;
        version = stat.version;
        cversion = stat.cversion;
        aversion = stat.aversion;
        ephemeralOwner = stat.ephemeralOwner;
        dataLength = stat.dataLength;
        numChildren = stat.numChildren;
        pzxid = stat.pzxid;
        return *this;
    }
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_STAT_H_
