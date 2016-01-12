#include <iostream>
#include <stdint.h>
using namespace std;

class RefCount{
public:
    RefCount():cnt(0){}
    RefCount(const RefCount& copy):cnt(0){}
    RefCount& operator=(const RefCount& copy)
    {
        return *this;
    }
    void IncRefCount()
    {
        cnt++;
    }
    void DecRefCount()
    {
        if(--cnt == 0){
            delete this;
        }
    }
    virtual ~RefCount() = 0;
private:
    uint32_t cnt;

};

RefCount::~RefCount()
{
}

//T must support refcount,must be RefCount's base
template<typename T> class SmartPtr{
public:
    SmartPtr(T* ptr);
    SmartPtr();
    SmartPtr(const SmartPtr& copy);
    SmartPtr& operator=(const SmartPtr& copy);
    T& operator*()const; //for const smartptr use the * operator
    T* operator->()const;//for const smartptr use the -> operator
    ~SmartPtr();
    T* Get()const;
private:
    void Copy(const SmartPtr& copy);
private:
    T* ptr_;
};

template<typename T> SmartPtr<T>::SmartPtr()
{
    ptr_ = NULL;
}

template<typename T> SmartPtr<T>::SmartPtr(T* ptr)
{
    ptr_ = ptr;
    ptr_->IncRefCount();
}

template<typename T> void SmartPtr<T>::Copy(const SmartPtr& copy)
{
    if(this == &copy)
        return;
    if(ptr_)
        ptr_->DecRefCount();
    ptr_ = copy.Get(); 
    ptr_->IncRefCount();
}

template<typename T>  SmartPtr<T>::SmartPtr(const SmartPtr& copy)
{
    Copy(copy);
}

template<typename T> SmartPtr<T>& SmartPtr<T>::operator=(const SmartPtr& copy)
{
    Copy(copy);
    return *this;
}

template<typename T> T* SmartPtr<T>::Get()const
{
    return ptr_;
}

template<typename T> T& SmartPtr<T>::operator *()const
{
    return *ptr_;
}

template<typename T> T* SmartPtr<T>::operator ->()const
{
    return ptr_;
}

template<typename T> SmartPtr<T>::~SmartPtr()
{
    ptr_->DecRefCount();
}
//use example
class Test: public RefCount{
public:
    virtual  ~Test()
    {
        cout << "test deconstruct" << endl;
    }
};

int main(int argc, char** argv)
{
    SmartPtr<Test>   test(new Test);
    SmartPtr<Test>   test1;

    test1 = test;
    return 0;
}
