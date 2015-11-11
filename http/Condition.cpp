#include "Condition.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
Condition::Condition(struct event_base* base):pipe_event_(base),base_(base)
{
    pipes_[0] = -1;
    pipes_[1] = -1;
}

Condition::~Condition()
{
    if(pipes_[0] != -1)
        close(pipes_[0]);
    if(pipes_[1] != -1)        
        close(pipes_[1]);
}

void Condition::Init()
{
#if 1
    if(pipe(pipes_) < 0)
        perror("create pipes error");
#else
    if(socketpair(AF_UNIX, SOCK_STREAM, 0, pipes_) < 0){
        perror("socketpair error");
    }
#endif
    int flags = 0;
    if ((flags = fcntl(pipes_[0], F_GETFL, 0)) == -1)
         flags = 0;
    fcntl(pipes_[0], F_SETFL, flags | O_NONBLOCK);
    if ((flags = fcntl(pipes_[1], F_GETFL, 0)) == -1)
         flags = 0;
    fcntl(pipes_[1], F_SETFL, flags | O_NONBLOCK);
}

void Condition::Watch(functor callback, uint64_t timeout)
{
    if(timeout == 0){
        pipe_event_.AsyncWait(pipes_[0],FdEvent::kReadable, std::tr1::bind(&Condition::CallBack, this, std::tr1::placeholders::_1));  
    }else{
    }

    handler_ = callback;
}

void Condition::Notify()
{
    char buf[1]={};

    write(pipes_[1], buf, sizeof(buf));
}

void Condition::CallBack(int fd)
{
    int n = 0;
    char buf[128] = {};

    while(1){ 
        n = read(pipes_[0], buf, 1);
        if(n > 0){
            handler_();
        }else if (n < 0){
            if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN){
                break;
            }
        }
    }
    pipe_event_.AsyncWait(pipes_[0],FdEvent::kReadable, std::tr1::bind(&Condition::CallBack, this, std::tr1::placeholders::_1));  

}
