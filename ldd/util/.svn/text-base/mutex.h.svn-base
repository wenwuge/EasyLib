#ifndef LDD_UTIL_MUTEX_H_
#define LDD_UTIL_MUTEX_H_

namespace ldd {
namespace util {

class Mutex {
public:
    Mutex();
    ~Mutex();

    void Lock();
    void Unlock();
    void AssertHeld();

private:
    friend class CondVar;
    struct Internal;
    Internal* internal_;
    // No copying allowed
    Mutex(const Mutex&);
    void operator=(const Mutex&);
};

class MutexLock {
public:
    explicit MutexLock(Mutex *mu): mu_(mu) { this->mu_->Lock(); }
    ~MutexLock() { this->mu_->Unlock(); }
private:
    Mutex* const mu_;
    // No copying allowed
    MutexLock(const MutexLock&);
    void operator=(const MutexLock&);
};

class CondVar {
public:
    explicit CondVar(Mutex* mu);
    ~CondVar();
    void Wait();
    void Signal();
    void SignalAll();
private:
    struct Internal;
    Internal* internal_;
    Mutex* mu_;
    // No copying allowed
    CondVar(const CondVar&);
    void operator=(const CondVar&);
};

class RWMutex {
public:
    RWMutex();
    ~RWMutex();
    void RLock();
    void WLock();
    void Unlock();
private:
    struct Internal;
    Internal* internal_;
    // No copying allowed
    RWMutex(const RWMutex&);
    void operator=(const RWMutex&);
};

class RWMutexLock {
public:
    RWMutexLock(RWMutex* mu, bool write = false)
        : mu_(mu) {
        if (write) {
            mu_->WLock();
        } else {
            mu_->RLock();
        }
    }
    ~RWMutexLock() {
        mu_->Unlock();
    }
private:
    RWMutex* mu_;
    // No copying allowed
    RWMutexLock(const RWMutexLock&);
    void operator=(const RWMutexLock&);
};

} // namespace util
} // namespace ldd

#endif // LDD_UTIL_MUTEX_H_
