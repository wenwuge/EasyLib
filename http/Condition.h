#ifndef __CONDITION_H
#define __CONDITION_H
#include "../thread/easyevent.h"
#include <unistd.h>
#include <tr1/functional>
#include <tr1/memory>
class Condition{
    typedef std::tr1::function<void()> functor;
    public:
        Condition(struct event_base* base);
        ~Condition();
        void Init();
        void Watch(functor callback,uint64_t timeout);
        void Notify();
    private:
        void CallBack(int fd);
    private:
        FdEvent pipe_event_;
        int     pipes_[2];
        functor handler_;
        struct  event_base *base_;
};
#endif
