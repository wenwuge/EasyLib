#include <iostream>
#include <map>
#include <pthread.h>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
using namespace std;

class Factory;

class MutexGuard{
public:
    MutexGuard(pthread_mutex_t& lock):lock_(lock){
        pthread_mutex_lock(&lock_);
    }
    ~MutexGuard(){
        pthread_mutex_unlock(&lock_);
    }
private:
    MutexGuard(const MutexGuard& copy):lock_(copy.lock_){}
    pthread_mutex_t& lock_;
};

class Object{
public:
    Object(string name, boost::shared_ptr<Factory> factory)
        :key_(name),factory_(factory){}
    ~Object();

private:
    string key_;
    boost::shared_ptr<Factory> factory_;
    
};

typedef boost::shared_ptr<Object> ObjectPtr;
typedef boost::weak_ptr<Object> ObjectWeakPtr;

class Factory:public boost::enable_shared_from_this<Factory>
{
public: 
    Factory(){
        pthread_mutex_init(&objects_lock_, NULL);
    }

    ~Factory(){
        cout<< "factory destruct" << endl;
    }

    ObjectPtr GetObject(string name){
        MutexGuard guard(objects_lock_); 
        ObjectPtr obj_ptr = (ObjectPtr)objects_[name].lock();
        if(!obj_ptr){
            obj_ptr.reset(new Object(name, shared_from_this() ));
            objects_[name] = obj_ptr;
        }
        
        return obj_ptr;
    }

    void UnregisterObject(const string &name)
    {
        MutexGuard guard(objects_lock_); 
        objects_.erase(name);
    }
private:
    map<string, ObjectWeakPtr> objects_;
    pthread_mutex_t  objects_lock_;

};

Object::~Object(){
    cout<< "object destruct" << endl;
    factory_->UnregisterObject(key_);
}

int main(int argc, char** argv)
{
    {
        boost::shared_ptr<Factory> factory(new Factory());
        ObjectPtr   obj1 = factory->GetObject("libin"); 
        ObjectPtr   obj2 = factory->GetObject("xiaoniwa"); 
    }    
    return 0;
}

