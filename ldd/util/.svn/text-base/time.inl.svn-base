// time.inl (2013-08-14)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_UTIL_TIME_INL_H
#define LDD_UTIL_TIME_INL_H

namespace ldd {
namespace util {

// ticks -> timespec/timeval
inline void Time::MillisecondsTo(int64_t ms, struct timespec* result) {
    TicksTo(ms, kMillisecond, result);
}
inline void Time::MillisecondsTo(int64_t ms, struct timeval* result) {
    TicksTo(ms, kMillisecond, result);
}
inline void Time::MicrosecondsTo(int64_t us, struct timespec* result) {
    TicksTo(us, kMicrosecond, result);
}
inline void Time::MicrosecondsTo(int64_t us, struct timeval* result) {
    TicksTo(us, kMicrosecond, result);
}
inline void Time::NanosecondsTo(int64_t ns, struct timespec* result) {
    TicksTo(ns, kNanosecond, result);
}
inline void Time::NanosecondsTo(int64_t ns, struct timeval* result) {
    TicksTo(ns, kNanosecond, result);
}
inline void Time::SecondsTo(int64_t s, struct timeval* result) {
    TicksTo(s, kSecond, result);
}
inline void Time::SecondsTo(int64_t s, struct timespec* result) {
    TicksTo(s, kSecond, result);
}
inline void Time::TicksTo(int64_t ticks, int64_t unit,
        struct timespec* result) {
    result->tv_sec = ticks / unit;
    int64_t ns_per_ticks = kNanosecond / unit;
    result->tv_nsec = (ticks % unit) * ns_per_ticks;
}
inline void Time::TicksTo(int64_t ticks, int64_t unit,
        struct timeval* result) {
    result->tv_sec = ticks / unit;
    int64_t us_per_ticks = kMicrosecond / unit;
    result->tv_usec = (ticks % unit) * us_per_ticks;
}

// timespec/timeval -> ticks
inline int64_t Time::ToMilliseconds(const struct timespec& t) {
    return ToTicks(t, kMillisecond);
}
inline int64_t Time::ToMilliseconds(const struct timeval& t) {
    return ToTicks(t, kMillisecond);
}
inline int64_t Time::ToMicroseconds(const struct timespec& t) {
    return ToTicks(t, kMicrosecond);
}
inline int64_t Time::ToMicroseconds(const struct timeval& t) {
    return ToTicks(t, kMicrosecond);
}
inline int64_t Time::ToNanoseconds(const struct timespec& t) {
    return ToTicks(t, kNanosecond);
}
inline int64_t Time::ToNanoseconds(const struct timeval& t) {
    return ToTicks(t, kNanosecond);
}
inline int64_t Time::ToSeconds(const struct timespec& t) {
    return ToTicks(t, kSecond);
}
inline int64_t Time::ToSeconds(const struct timeval& t) {
    return ToTicks(t, kSecond);
}
inline int64_t Time::ToTicks(const struct timespec& t, int64_t unit) {
    return ToTicks(t.tv_sec, t.tv_nsec, kNanosecond, unit);
}
inline int64_t Time::ToTicks(const struct timeval& t, int64_t unit) {
    return ToTicks(t.tv_sec, t.tv_usec, kMicrosecond, unit);
}

inline int64_t Time::TicksToTicks(int64_t ticks,
        int64_t unit, int64_t new_unit) {
    return ToTicks(ticks / unit, ticks % unit, unit, new_unit);
}

inline int64_t Time::CurrentSeconds() {
    return CurrentTicks(kSecond);
}
inline int64_t Time::CurrentMilliseconds() {
    return CurrentTicks(kMillisecond);
}
inline int64_t Time::CurrentMicroseconds() {
    return CurrentTicks(kMicrosecond);
}
inline int64_t Time::CurrentNanoseconds() {
    return CurrentTicks(kNanosecond);
}

inline int64_t Time::ToTicks(int64_t seconds, int64_t old_ticks,
               int64_t old_unit, int64_t new_unit) {
    int64_t result = seconds * new_unit;
    result += old_ticks * new_unit / old_unit;
    int64_t old_per_new = old_unit / new_unit;
    if ((old_per_new > 1)
            && ((old_ticks % old_per_new) >= (old_per_new / 2))) {
        ++result;
    }
    return result;
}

} // namespace util
} // namespace ldd

#endif // LDD_UTIL_TIME_INL_H

