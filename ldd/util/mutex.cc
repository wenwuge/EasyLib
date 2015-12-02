#include <pthread.h>
#include <string.h>
#include <glog/logging.h>
#include "mutex.h"

namespace ldd {
namespace util {

struct Mutex::Internal {
    pthread_mutex_t mutex;
};

Mutex::Mutex()
    : internal_(new Internal) {
    pthread_mutex_init(&internal_->mutex, NULL);
}

Mutex::~Mutex() {
    pthread_mutex_destroy(&internal_->mutex);
    delete internal_;
}

void Mutex::Lock() {
    int result = pthread_mutex_lock(&internal_->mutex);
    if (result != 0) {
        LOG(FATAL) << "pthread_mutex_lock: " << strerror(result);
    }
}

void Mutex::Unlock() {
    int result = pthread_mutex_unlock(&internal_->mutex);
    if (result != 0) {
        LOG(FATAL) << "pthread_mutex_lock: " << strerror(result);
    }
}

void Mutex::AssertHeld() {
}

struct RWMutex::Internal {
    pthread_rwlock_t mutex;
};

struct CondVar::Internal {
    pthread_cond_t cond;
};

CondVar::CondVar(Mutex* mu)
    :internal_(new Internal), mu_(mu) {
    pthread_cond_init(&internal_->cond, NULL);
}

CondVar::~CondVar() {
    pthread_cond_destroy(&internal_->cond);
    delete internal_;
}

void CondVar::Wait() {
    int result = pthread_cond_wait(&internal_->cond, &mu_->internal_->mutex);
    if (result != 0) {
        LOG(FATAL) << "pthread_cond_wait: " << strerror(result);
    }
}

void CondVar::Signal() {
    int result = pthread_cond_signal(&internal_->cond);
    if (result != 0) {
        LOG(FATAL) << "pthread_cond_signal: " << strerror(result);
    }
}

void CondVar::SignalAll() {
    int result = pthread_cond_broadcast(&internal_->cond);
    if (result != 0) {
        LOG(FATAL) << "pthread_cond_broadcast: " << strerror(result);
    }
}

RWMutex::RWMutex()
    : internal_(new Internal) {
    pthread_rwlock_init(&internal_->mutex, NULL);
}

RWMutex::~RWMutex() {
    pthread_rwlock_destroy(&internal_->mutex);
    delete internal_;
}

void RWMutex::RLock() {
    int result = pthread_rwlock_rdlock(&internal_->mutex);
    if (result != 0) {
        LOG(FATAL) << "pthread_rwlock_rdlock: " << strerror(result);
    }
}

void RWMutex::WLock() {
    int result = pthread_rwlock_wrlock(&internal_->mutex);
    if (result != 0) {
        LOG(FATAL) << "pthread_rwlock_wrlock: " << strerror(result);
    }
}

void RWMutex::Unlock() {
    int result = pthread_rwlock_unlock(&internal_->mutex);
    if (result != 0) {
        LOG(FATAL) << "pthread_rwlock_unlock: " << strerror(result);
    }
}

} // namespace util
} // namespace ldd
