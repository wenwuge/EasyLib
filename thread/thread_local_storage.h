#ifndef DONG_BASE_THREADLOCAL_H
#define DONG_BASE_THREADLOCAL_H

#include <map>
#include <string>
#define TlsGetValue pthread_getspecific 
#define TlsSetValue pthread_setspecific 
#define TlsFree     pthread_key_delete
#include <pthread.h>
#include "../common/noncopyable.h"
using namespace std;

template<typename T>
class ThreadLocalStorage : Noncopyable
{
public:
    typedef std::map<string, T> StringMap;
public:
    ThreadLocalStorage()
    {
        pthread_key_create(&thread_key_, &ThreadLocalStorage::destructor);
    }

    ~ThreadLocalStorage()
    {
        TlsFree(thread_key_);
    }

    T& value(const string& key)
    {
        StringMap* perThreadValue = static_cast<StringMap*>(TlsGetValue(thread_key_));
        if (!perThreadValue) {
            StringMap* newObj = new StringMap();
            TlsSetValue(thread_key_, newObj);
            perThreadValue = newObj;
        }
        return (*perThreadValue)[key];
    }

private:

    static void destructor(void *x)
    {
        StringMap* obj = static_cast<StringMap*>(x);
        typedef char T_must_be_complete_type[sizeof(StringMap) == 0 ? -1 : 1];
        T_must_be_complete_type dummy; (void) dummy;
        delete obj;
    }

private:
    pthread_key_t thread_key_;
};


template<typename T>
class ThreadLocal: Noncopyable
{
public:
    ThreadLocal()
    {
        pthread_key_create(&thread_key_, &ThreadLocal::destructor);
    }

    ~ThreadLocal()
    {
        TlsFree(thread_key_);
    }

    T& value()
    {
        T* perThreadValue = static_cast<T*>(TlsGetValue(thread_key_));
        if (!perThreadValue) {
            T* newObj = new T();
            TlsSetValue(thread_key_, newObj);
            perThreadValue = newObj;
        }
        return *perThreadValue;
    }

    operator T&()
    {
        return value();
    }

private:

    static void destructor(void *x)
    {
        T* obj = static_cast<T*>(x);
        typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
        T_must_be_complete_type dummy; (void) dummy;
        delete obj;
    }

private:
    pthread_key_t thread_key_;
};
#endif



