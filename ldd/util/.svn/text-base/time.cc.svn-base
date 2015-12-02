#include <glog/logging.h>
#include "time.h"

namespace ldd {
namespace util {

/*
int64_t Time::CurrentTicks(int64_t ticks_per_second) {
    struct timespec now;
    int ret = clock_gettime(CLOCK_REALTIME, &now);
    CHECK_EQ(ret, 0);
    return ToTicks(now, ticks_per_second);
}
*/

int64_t Time::CurrentTicks(int64_t ticks_per_second) {
    struct timeval now;
    int ret = gettimeofday(&now, NULL);
    CHECK_EQ(ret, 0);
    if (ticks_per_second == kMicrosecond) {
        return now.tv_sec * kMicrosecond + now.tv_usec;
    } else {
        return ToTicks(now, ticks_per_second);
    }
}

} // namespace util
} // namespace ldd
