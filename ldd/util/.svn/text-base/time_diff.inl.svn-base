// time_diff.inl (2013-08-14)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_UTIL_TIME_DIFF_INL_
#define LDD_UTIL_TIME_DIFF_INL_

namespace ldd {
namespace util {

inline TimeDiff::TimeDiff()
    : us_(0)
{
}

inline bool TimeDiff::IsZero() const {
    return us_ == 0;
}

inline TimeDiff::TimeDiff(int64_t ticks, int64_t unit)
    : us_(Time::TicksToTicks(ticks, unit, Time::kMicrosecond))
{
}
inline TimeDiff::TimeDiff(int64_t us)
    : us_(us)
{
}

// Constructors from timeval/timespec
inline TimeDiff::TimeDiff(const struct timespec& t)
    : us_(Time::ToMicroseconds(t))
{
}
inline TimeDiff::TimeDiff(const struct timeval& t)
    : us_(Time::ToMicroseconds(t))
{
}

// Factory methods from *seconds
inline TimeDiff TimeDiff::Seconds(int64_t s) {
    return TimeDiff(s, Time::kSecond);
}
inline TimeDiff TimeDiff::Milliseconds(int64_t ms) {
    return TimeDiff(ms, Time::kMillisecond);
}
inline TimeDiff TimeDiff::Microseconds(int64_t us) {
    return TimeDiff(us);
}
inline TimeDiff TimeDiff::Nanoseconds(int64_t ns) {
    return TimeDiff(ns, Time::kNanosecond);
}

// Property getters
inline int64_t TimeDiff::ToSeconds() const {
    return Time::TicksToTicks(us_, Time::kMicrosecond, Time::kSecond);
}
inline int64_t TimeDiff::ToMilliseconds() const {
    return Time::TicksToTicks(us_, Time::kMicrosecond, Time::kMillisecond);
}
inline int64_t TimeDiff::ToMicroseconds() const {
    return us_;
}
inline int64_t TimeDiff::ToNanoseconds() const {
    return Time::TicksToTicks(us_, Time::kMicrosecond, Time::kNanosecond);
}

// Converters
inline void TimeDiff::To(struct timespec* t) const {
    Time::MicrosecondsTo(us_, t);
}
inline void TimeDiff::To(struct timeval* t) const {
    Time::MicrosecondsTo(us_, t);
}

// Comparator
inline bool TimeDiff::operator< (const TimeDiff& rhs) const {
    return us_ < rhs.us_;
}
inline bool TimeDiff::operator==(const TimeDiff& rhs) const {
    return us_ == rhs.us_;
}

// Math operator
inline TimeDiff TimeDiff::operator+=(const TimeDiff& rhs) {
    us_ += rhs.us_;
    return *this;
}
inline TimeDiff TimeDiff::operator-=(const TimeDiff& rhs) {
    us_ -= rhs.us_;
    return *this;
}
inline TimeDiff TimeDiff::operator*=(int n) {
    us_ *= n;
    return *this;
}
inline TimeDiff TimeDiff::operator/=(int n) {
    us_ /= n;
    return *this;
}

} // namespace util
} // namespace ldd
#endif // LDD_UTIL_TIME_DIFF_INL_
