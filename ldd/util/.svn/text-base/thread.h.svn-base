#ifndef LDD_UTIL_THREAD_H_
#define LDD_UTIL_THREAD_H_

#include <stdint.h>
#include <tr1/functional>

namespace ldd {
namespace util {

class Thread {
public:
    typedef uint64_t Id;
    typedef std::tr1::function<void()> Function;
    Thread(const Function& function);
    ~Thread();

    bool Start();
    void Join();

    Id id() const;

    static Id CurrentId();
    static void SleepForMs(int64_t milliseconds);
    static void SleepForUs(int64_t microseconds);
    static void SleepForNs(int64_t nanoseconds);
    static void Yield();
private:
    struct Internal;
    Internal* internal_;
    Function function_;
    static void* ThreadMain(void*);
    // No copying allowed
    Thread(const Thread&);
    void operator=(const Thread&);
};

} // namespace util
} // namespace ldd

#endif // LDD_UTIL_THREAD_H_
