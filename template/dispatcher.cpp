#include <iostream>
#include <map>
#include <string>
#include <boost/function.hpp>
#include <boost/bind.hpp>
using namespace std;

class Message{
};

class Message1:public Message{
};

class Message2:public Message{
};

class CallbackBase{
public:
    virtual void output(Message* message) = 0;
};

template <typename T> class CallbackT: public CallbackBase{
public:
    typedef boost::function<void(T*)> CallbackFunc; 
    CallbackT(CallbackFunc func):callback_(func){}
    void output(Message* message)
    {
        callback_(static_cast<T*>(message));
    }
private:
    CallbackFunc callback_;
};

class Dispatcher{
public:
    //use template to solve the params question.
    template <typename T> void RegisterCallback(int id , typename CallbackT<T>::CallbackFunc callback)
    {
        //using base and derived class to resolve the callback storage question
        callback_map_[id] = new CallbackT<T>(callback);
    }
    void Run(int id);
private:
    std::map<int, CallbackBase*> callback_map_;
};

void Dispatcher::Run(int id)
{
    switch (id){
    case 1: 
        {
            Message1* message1 = new Message1;
            callback_map_[1]->output(message1);
        }
        break;
    case 2:
        {
            Message2* message2 = new Message2;
            callback_map_[2]->output(message2);
        }
        break;
    default:
        cout << "not found id" << endl;
        break;
    }
}

void OutputMessage1(Message1 * message)
{
    cout << "output message1" << endl;
}

void OutputMessage2(Message2 * message)
{
    cout << "outout message2" << endl;
}

int main(int argc, char** argv)
{
    Dispatcher dispatcher;

    dispatcher.RegisterCallback<Message1>(1, boost::bind(&OutputMessage1, _1));
    dispatcher.RegisterCallback<Message2>(2, boost::bind(&OutputMessage2, _1));

    dispatcher.Run(1);
    dispatcher.Run(2);
    return 0;
}
