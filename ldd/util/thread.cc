#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <glog/logging.h>
#include "thread.h"
#include "time.h"

namespace ldd {
namespace util {

struct Thread::Internal {
    pthread_t t;
};

Thread::Thread(const Function& function)
    : internal_(NULL),
      function_(function) {
}

Thread::~Thread() {
    if (internal_) {
        Join();
    }
}

Thread::Id Thread::id() const {
    CHECK_NOTNULL(internal_);
    return (Id)internal_->t;
}

bool Thread::Start() {
    CHECK_EQ(internal_, (void*)NULL);
    CHECK(function_);

    internal_ = new Internal;
    int result = pthread_create(&internal_->t, NULL, Thread::ThreadMain,
            (void*)this);
    if (result != 0) {
        delete internal_;
        LOG(ERROR) << "pthread_create: " << strerror(result);
        return false;
    }
    return true;
}

void Thread::Join() {
    CHECK_NOTNULL(internal_);

    int result = pthread_join(internal_->t, NULL);
    if (result != 0) {
        LOG(FATAL) << "pthread_join: " << strerror(result);
    }
    delete internal_;
    internal_ = NULL;
}

void* Thread::ThreadMain(void* arg) {
    Thread* thread = static_cast<Thread*>(arg);
    try {
        thread->function_();
    } catch (const std::exception& e) {
        LOG(FATAL) << "ldd::util::ThreadMain throws: " << e.what();
    } catch (...) {
        LOG(FATAL) << "ldd::util::ThreadMain throws";
    }
    return NULL;
}

Thread::Id Thread::CurrentId() {
    return (Id)pthread_self();
}

void Thread::SleepForMs(int64_t milliseconds) {
    int result = usleep(milliseconds * 1000);
    if (result != 0 && errno != EINTR) {
        LOG(FATAL) << "usleep: " << strerror(errno);
    }
}

void Thread::SleepForUs(int64_t microseconds) {
    int result = usleep(microseconds);
    if (result != 0 && errno != EINTR) {
        LOG(FATAL) << "usleep: " << strerror(errno);
    }
}

void Thread::SleepForNs(int64_t nanoseconds) {
    struct timespec ts;
    Time::NanosecondsTo(nanoseconds, &ts);
    nanosleep(&ts, NULL);
}

void Thread::Yield() {
#if defined(_POSIX_PRIORITY_SCHEDULING)
    sched_yield();
#else
    SleepForNs(0);
#endif
}

} // namespace util
} // namespace ldd
