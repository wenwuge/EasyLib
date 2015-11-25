#include <iostream>
using namespace std;

class Action{
    public:
        virtual void Start() = 0;
        virtual void Move() = 0;
};

class Movement{
    public:
        Movement(Action* action)
        {    
            action_ = action;
        }
        void Move()
        {
            action_->Start();
            action_->Move();
        }
        virtual ~Movement(){}
    private:
        Action * action_;
};

class Person: public Movement{
    public:
        Person(Action * action):Movement(action)
        {
        }

};

class Car: public Movement{
    public:
        Car(Action * action):Movement(action)
        {
        }
};

class PersonAct: public Action{
    public:
        virtual void Start()
        {
            cout << "person start" << endl;
        }
        virtual void Move()
        {
            cout << "person Move" << endl;
        }
};

class CarAct: public Action{
    public:
        virtual void Start()
        {
            cout << "car start" << endl;
        }
        virtual void Move()
        {
            cout << "car Move" << endl;
        }
};

int main(int argc, char** argv)
{
    PersonAct pact;
    CarAct cact;

    Person person(&pact);
    Car   car(&cact);

    person.Move();
    car.Move();
}
