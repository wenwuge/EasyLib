#ifndef __MESSAGE_FACTORY
#define __MESSAGE_FACTORY
#include <iostream>
#include <map>
using namespace std;
struct NoneArg;
class BaseMessage{

};

class Message1 : public BaseMessage{
public:
    Message1(string a1)
    {}
};

class Message2: public BaseMessage{
public:
    Message2(string a1, string b1)
    {
        cout << "new message2 " << endl;
    }

};

class Factory {
public:
    virtual BaseMessage* GetMessage() = 0;
};
template<class Message , class A1, class A2> class factory:public Factory{
public:
    factory(A1  a1, A2 a2):a1_(a1),a2_(a2){};
    Message* GetMessage()
    {
        cout << "get message2 " << endl;
        return new Message(a1_, a2_); 
    }
private:
    A1 a1_;
    A2 a2_;
};


template<class Message , class A1> class factory<Message, A1, NoneArg>:public Factory{
public:
    factory(A1  a1):a1_(a1){};
    Message* GetMessage()
    {
        cout << "get message1 " << endl;
        return new Message(a1_); 
    }

private:
    A1 a1_;
};



template<typename Message> class MessageFactories{
public:
    Message* GetMessage(int type)
    {
        return factories_[type]->GetMessage();
    }

    template <typename DerivedMessage,typename A1> 
        void Register(int type, A1 a1)
    {
        Factory* _factory = NULL;
        _factory = new factory<DerivedMessage,A1,NoneArg>(a1);

        factories_[type] = _factory;
    }
    template <typename DerivedMessage, typename A1, typename A2> 
        void Register(int type, A1 a1 , A2 a2)
    {

        Factory* _factory = NULL;
        _factory = new factory<DerivedMessage,A1,A2>(a1,a2);
        factories_[type] = _factory;
    }
private:
    map<int, Factory*> factories_;
};


#endif
