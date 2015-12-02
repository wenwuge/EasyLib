/**************************************************************************
 *author:李斌
 *日期：2015/9/7
 *说明：TCP服务器线程池，基于libevent编写，使用该类只需注册个处理函数即可，
 *在处理函数中可以进行数据的发送，接收/发送完全异步
 *
 */
#ifndef __THREAD_POOL
#define __THREAD_POOL
#include <string>
#include <pthread.h>
#include <vector>
#include <event.h>
#include <tr1/functional>
#include <tr1/memory>
#include <memory>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <list>
#include <stdlib.h>

using namespace std;
typedef unsigned short Uint16;
typedef unsigned char  Uint8;

#define TCP_MESSAGE_LEN 1500
#define MAXRECVLEN 1500

class Thread;

struct BufDesc{
    BufDesc(char* buf, int len):len_(0), pos_(0)
    {
        if(buf == NULL || len <=0 || len >MAXRECVLEN){
            cout << "buf or len error" << endl;
            return;
        }

        memcpy(buf_, buf, len);
        len_ = len;
    }

    char   buf_[MAXRECVLEN];
    Uint16 len_;
    Uint16 pos_;
};

typedef tr1::shared_ptr<BufDesc> BufDescPtr;
enum{
    EVENT_OFF =0X0,
    READ_EVENT_ON = 0x1,
    WRITE_EVENT_ON = 0X2
};
struct Client{

    Client(Thread* thread, struct event_base* base_)
    {
        thread_ = NULL;
        base = NULL;

        thread_ = thread;
        base = base_;
        EventFlag = EVENT_OFF;
        pEvRead = (struct event*)malloc(sizeof(struct event)); 
        pEvWrite =(struct event*)malloc(sizeof(struct event)); 
    }
    
    ~Client()
    {
        //detach read event from libevent
        if(EventFlag &READ_EVENT_ON){
            event_del(pEvRead);
        }
        free(pEvRead);
        
        //detach write evnet from libevent
        if(EventFlag & WRITE_EVENT_ON){
            event_del(pEvWrite);
        }
        free(pEvWrite);
        sendBufList.clear();
    }

    Thread* thread_;
    Uint8  EventFlag;
    struct event * pEvRead;
    struct event * pEvWrite;
    struct event_base* base;
    list<BufDescPtr> sendBufList;
};


class  TcpMessageDesc{

    public:
        TcpMessageDesc():dataLen(0),bufLen(TCP_MESSAGE_LEN),
                        fd(0), client_(NULL){};
        char* GetBufPtr();
        int   GetDataLen();
        void   SetDataLen(int len);
        void  SetClient(Client* client);
        void  SetFd(int fd_);
        Client* GetClient();
        int   GetFd();
    private:
        char  buf[TCP_MESSAGE_LEN];
        int   dataLen;
        int   bufLen;
        
        //for send data, keep thread and event_base info
        int fd;
        struct Client* client_;


};

typedef tr1::shared_ptr<TcpMessageDesc> TcpMessageDescPtr;

typedef std::tr1::function<void(TcpMessageDescPtr&)> WorkFunction;


struct ConnInfo{
    int fd;
    struct sockaddr_in addr;
};

class Thread{


    enum{
        STATE_START,
        STATE_STOP,
        STATE_RUN
    };

    public:
        Thread(int read_fd, WorkFunction func, Uint16 timeout = 0);
        bool Start();
        bool Stop();
        void EventBaseDispatch();
        void ProcessNewConnect(ConnInfo* info);
        void DoUserWork(TcpMessageDescPtr& tcpPtr);

        static void OnAccept(int fd, short event, void *arg);
        static void* Process(void * param);
        static void OnRead(int fd, short event, void *arg);
        static void OnWrite(int fd, short event, void *arg);
        static int Send(TcpMessageDescPtr tcpDesc, char* data, int len);

        static void SetWriteEvent(struct event_base* base, struct event* write_event, 
                            int fd, void* arg);        
    private:
        WorkFunction func_;
        int tid_;
        Uint8 state_;
        int readfd_;
        Uint16 timeout_;
        struct event_base* base;
};

typedef tr1::shared_ptr<Thread> ThreadPtr;
struct ThreadInfo{
    ThreadPtr thread_;
    int  writefd_;
};

class Threadpool{
 public:
    Threadpool(Uint16 thread_num);
    ~Threadpool(){};
    void RegistersJob(WorkFunction job);
    bool Start(Uint16 port_, Uint16 timeout = 0);
    bool Stop();

 private:
    bool CreateThreads();
    bool StartThreads();

 private:
    Uint16  thread_num_;
    Uint16  next_;
    Uint16  port_;
    vector<ThreadInfo> threads_;
    WorkFunction job_;
};
#endif
