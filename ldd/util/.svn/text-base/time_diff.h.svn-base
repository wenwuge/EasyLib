// time_diff.h (2013-08-14)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_UTIL_TIME_DIFF_H_
#define LDD_UTIL_TIME_DIFF_H_

#include <boost/operators.hpp>
#include "ldd/util/time.h"

namespace ldd {
namespace util {

class TimeDiff : boost::less_than_comparable<TimeDiff>,
                 boost::equality_comparable<TimeDiff>,
                 boost::addable<TimeDiff>,
                 boost::subtractable<TimeDiff>,
                 boost::multipliable<TimeDiff, int>,
                 boost::dividable<TimeDiff, int>
{
    friend class Timestamp;
public:
    TimeDiff();
    // Constructors from timeval/timespec
    TimeDiff(const struct timespec& t);
    TimeDiff(const struct timeval& t);

    // Factory methods from *seconds
    static TimeDiff Seconds(int64_t s);
    static TimeDiff Milliseconds(int64_t ms);
    static TimeDiff Microseconds(int64_t us);
    static TimeDiff Nanoseconds(int64_t ns);

    // Property getters
    int64_t ToSeconds() const;
    int64_t ToMilliseconds() const;
    int64_t ToMicroseconds() const;
    int64_t ToNanoseconds() const;

    // Converters
    void To(struct timespec* t) const;
    void To(struct timeval* t) const;

    // Comparator
    bool IsZero() const;
    bool operator< (const TimeDiff& rhs) const;
    bool operator==(const TimeDiff& rhs) const;

    // Math operator
    TimeDiff operator+=(const TimeDiff& rhs);
    TimeDiff operator-=(const TimeDiff& rhs);
    TimeDiff operator*=(int n);
    TimeDiff operator/=(int n);
public:
    TimeDiff(int64_t ticks, int64_t unit);
private:
    explicit TimeDiff(int64_t us);
    int64_t us_;
};

} // namespace util
} // namespace ldd

#include "time_diff.inl"

#endif // LDD_UTIL_TIME_DIFF_H_
