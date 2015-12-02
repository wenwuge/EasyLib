// timestamp.inl (2013-08-14)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_UTIL_TIMESTAMP_INL_
#define LDD_UTIL_TIMESTAMP_INL_

namespace ldd {
namespace util {

inline Timestamp::Timestamp()
    : us_(0)
{
}

inline Timestamp Timestamp::Epoch() {
    return Timestamp();
}

inline bool Timestamp::IsEpoch() const {
    return us_ == 0;
}

inline Timestamp::Timestamp(int64_t us)
    : us_(us)
{
}

inline Timestamp::Timestamp(int64_t ticks, int64_t unit)
    : us_(Time::TicksToTicks(ticks, unit, Time::kMicrosecond))
{
}

inline Timestamp::Timestamp(const struct timespec& t)
    : us_(Time::ToMicroseconds(t))
{
}

inline Timestamp::Timestamp(const struct timeval& t)
    : us_(Time::ToMicroseconds(t))
{
}

inline Timestamp Timestamp::Seconds(int64_t s) {
    return Timestamp(s, Time::kSecond);
}
inline Timestamp Timestamp::Milliseconds(int64_t ms) {
    return Timestamp(ms, Time::kMillisecond);
}
inline Timestamp Timestamp::Microseconds(int64_t us) {
    return Timestamp(us);
}
inline Timestamp Timestamp::Nanoseconds(int64_t ns) {
    return Timestamp(ns, Time::kNanosecond);
}

inline Timestamp Timestamp::Now() {
    return Timestamp(Time::CurrentMicroseconds());
}

inline int64_t Timestamp::ToSeconds() const {
    return Time::TicksToTicks(us_, Time::kMicrosecond, Time::kSecond);
}
inline int64_t Timestamp::ToMilliseconds() const {
    return Time::TicksToTicks(us_, Time::kMicrosecond, Time::kMillisecond);
}
inline int64_t Timestamp::ToMicroseconds() const {
    return us_;
}
inline int64_t Timestamp::ToNanoseconds() const {
    return Time::TicksToTicks(us_, Time::kMicrosecond, Time::kNanosecond);
}

inline void Timestamp::To(struct timespec* t) const {
    Time::MicrosecondsTo(us_, t);
}
inline void Timestamp::To(struct timeval* t) const {
    Time::MicrosecondsTo(us_, t);
}

// Comparator
inline bool Timestamp::operator< (const Timestamp& rhs) const {
    return us_ < rhs.us_;
}
inline bool Timestamp::operator==(const Timestamp& rhs) const {
    return us_ == rhs.us_;
}

// Math operator
inline Timestamp Timestamp::operator+=(const TimeDiff& rhs) {
    us_ += rhs.us_;
    return *this;
}

inline Timestamp Timestamp::operator+ (const TimeDiff& rhs) const {
    Timestamp temp(*this);
    temp += rhs;
    return temp;
}

inline Timestamp Timestamp::operator-=(const TimeDiff& rhs) {
    us_ -= rhs.us_;
    return *this;
}

inline Timestamp Timestamp::operator- (const TimeDiff& rhs) const {
    Timestamp temp(*this);
    temp -= rhs;
    return temp;
}

inline TimeDiff  Timestamp::operator- (const Timestamp& rhs) const {
    int64_t us = us_ - rhs.us_;
    return TimeDiff(us);
}


} // namespace util
} // namespace ldd

#endif // LDD_UTIL_TIMESTAMP_INL_
