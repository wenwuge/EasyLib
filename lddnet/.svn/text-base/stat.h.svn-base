// stat.h (2014-08-01)
// Yan Gaofeng (yangaofeng@360.cn)

#ifndef __STAT_H__
#define __STAT_H__

namespace ldd { namespace net {

class Stat {
public:
    ~Stat() {}

    std::string Status() const;
    static Stat &instance() { return instance_; }

private:
    Stat();
    static Stat instance_;
};

} /*namespace net*/
} /*namespace ldd*/

#endif /*__STAT_H__*/

