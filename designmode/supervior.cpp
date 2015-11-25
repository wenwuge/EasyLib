#include <iostream>
#include <list>
#include <algorithm>
using namespace std;

class Observer{
public:
    virtual void Update(int) = 0;
};

class Subject{
public:
    virtual void Attach(Observer*) = 0;
    virtual void DeAttach(Observer*) = 0;
    virtual void Notify() = 0;

};

class ConcreteObserver: public Observer{
public:
    virtual void Update(int state)
    {
        cout<< "state is " << state <<endl;
    }
    
    ConcreteObserver(Subject* subject):subject_(subject){}
private:
    Subject * subject_;
};

class ConcreteSubject: public Subject{
public:
    virtual void Attach(Observer* observer)  
    {
        observer_list.push_back(observer);
    }
    
    virtual void DeAttach(Observer* observer)
    {
        list<Observer*>::iterator it = observer_list.begin();
#if 0
        for(; it != observer_list.end(); it ++){
            if(*it == observer)
                break;
        }
#else
        it = find(observer_list.begin(), observer_list.end(), observer);
#endif
        if(it != observer_list.end()){
            observer_list.erase(it);
        }
    }
    
    virtual void Notify()
    {
        list<Observer*>::iterator it = observer_list.begin();

        for(; it != observer_list.end(); it ++){
            (*it)->Update(1);
        }
        
    }

private:
    list<Observer*> observer_list;
};


int main(int argc, char** argv)
{
    ConcreteSubject subject;
    ConcreteObserver observer1(&subject);
    ConcreteObserver observer2(&subject);

    subject.Attach(&observer1);
    subject.Attach(&observer2);
    subject.Notify();
    subject.DeAttach(&observer1);
    subject.Notify();

    return 0;
}
