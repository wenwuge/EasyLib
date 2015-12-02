#ifndef LDD_UTIL_TIME_H_
#define LDD_UTIL_TIME_H_

#include <stdint.h>
#include <time.h>
#include <sys/time.h>

namespace ldd {
namespace util {

class Time {
public:
    // Utility functions: Current time getter
    static int64_t CurrentSeconds();
    static int64_t CurrentMilliseconds();
    static int64_t CurrentMicroseconds();
    static int64_t CurrentNanoseconds();

    // Utility functions: Convert ticks -> timespec/timeval
    static void MillisecondsTo(int64_t ms, struct timespec* result);
    static void MillisecondsTo(int64_t ms, struct timeval* result);
    static void MicrosecondsTo(int64_t us, struct timespec* result);
    static void MicrosecondsTo(int64_t us, struct timeval* result);
    static void NanosecondsTo(int64_t ns, struct timespec* result);
    static void NanosecondsTo(int64_t ns, struct timeval* result);
    static void SecondsTo(int64_t s, struct timeval* result);
    static void SecondsTo(int64_t s, struct timespec* result);
    
    // Utility functions: Convert timespec/timeval -> ticks
    static int64_t ToMilliseconds(const struct timespec& t);
    static int64_t ToMilliseconds(const struct timeval& t);
    static int64_t ToMicroseconds(const struct timespec& t);
    static int64_t ToMicroseconds(const struct timeval& t);
    static int64_t ToNanoseconds(const struct timespec& t);
    static int64_t ToNanoseconds(const struct timeval& t);
    static int64_t ToSeconds(const struct timespec& t);
    static int64_t ToSeconds(const struct timeval& t);

public:
    // Underlaying methods
    enum Unit {
        kSecond      = 1LL,
        kMillisecond = 1000LL,
        kMicrosecond = 1000000LL,
        kNanosecond  = 1000000000LL,
    };
    // Utility functions: Current time getter
    static int64_t CurrentTicks(int64_t unit);
    // Utility functions: Convert ticks -> timespec/timeval
    static void TicksTo(int64_t ticks, int64_t unit,
            struct timespec* result);
    static void TicksTo(int64_t ticks, int64_t unit,
            struct timeval* result);

    // Utility functions: Convert timespec/timeval -> ticks
    static int64_t ToTicks(const struct timespec& t, int64_t unit);
    static int64_t ToTicks(const struct timeval& t, int64_t unit);
    static int64_t ToTicks(int64_t seconds, int64_t old_ticks,
                   int64_t old_unit, int64_t new_unit);

    // Utility functions: Convert ticks -> ticks
    static int64_t TicksToTicks(int64_t ticks,
            int64_t unit, int64_t new_unit);
};

} // namespace util
} // namespace ldd

#include "time.inl"

#endif // LDD_UTIL_TIME_H_
