#ifndef __DOUBLEBUFFER
#define __DOUBLEBUFFER
#include <pthread.h>
#include <stdlib.h>
#include <memory>
#include <tr1/memory>
#include <string>
using namespace std;

template <class T> class DoubleBuffer{
    typedef std::tr1::shared_ptr<T> T_ptr;

    public:
        DoubleBuffer(string name)
        {
            name_ = name;
        }
        
        T_ptr Get()
        {
            pthread_mutex_lock(&lock_);
            T_ptr tmp = ptr_;
            pthread_mutex_lock(&lock_);

            return tmp;
        }
        void  Reload(T_ptr& dataptr)
        {

            pthread_mutex_lock(&lock_);
            ptr_ = dataptr;
            pthread_mutex_lock(&lock_);
        }
    private:
        pthread_mutex_t  lock_;
        //void* ptr_;
        T_ptr ptr_;
        string name_;
};
#endif
