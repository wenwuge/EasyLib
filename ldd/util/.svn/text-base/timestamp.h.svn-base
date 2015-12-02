// timestamp.h (2013-08-14)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_UTIL_TIMESTAMP_H_
#define LDD_UTIL_TIMESTAMP_H_

#include <boost/operators.hpp>
#include "ldd/util/time.h"
#include "ldd/util/time_diff.h"

namespace ldd {
namespace util {

class Timestamp : boost::less_than_comparable<Timestamp>,
                  boost::equality_comparable<Timestamp>
{
public:
    // Constructor and factory to get the epoch timestamp
    Timestamp();
    static Timestamp Epoch();

    // Constructors from timeval/timespec
    Timestamp(const struct timespec& t);
    Timestamp(const struct timeval& t);

    // Factory methods from *seconds
    static Timestamp Seconds(int64_t s);
    static Timestamp Milliseconds(int64_t ms);
    static Timestamp Microseconds(int64_t us);
    static Timestamp Nanoseconds(int64_t ns);

    // Factory methods from current time
    static Timestamp Now();

    // Property getters
    int64_t ToSeconds() const;
    int64_t ToMilliseconds() const;
    int64_t ToMicroseconds() const;
    int64_t ToNanoseconds() const;

    // Converters
    void To(struct timespec* t) const;
    void To(struct timeval* t) const;

    // Comparator
    bool IsEpoch() const;
    bool operator< (const Timestamp& rhs) const;
    bool operator==(const Timestamp& rhs) const;

    // Math operator
    Timestamp operator+=(const TimeDiff& rhs);
    Timestamp operator+ (const TimeDiff& rhs) const;
    Timestamp operator-=(const TimeDiff& rhs);
    Timestamp operator- (const TimeDiff& rhs) const;
    TimeDiff operator- (const Timestamp& rhs) const;
public:
    Timestamp(int64_t ticks, int64_t unit);
private:
    explicit Timestamp(int64_t us);
    int64_t us_;
};

} // namespace util
} // namespace ldd

#include "timestamp.inl"

#endif // LDD_UTIL_TIMESTAMP_H_

