#include "threadpool.h"
#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

char* TcpMessageDesc::GetBufPtr()
{
    return buf;
}

int TcpMessageDesc::GetDataLen()
{
    return dataLen;
}

void TcpMessageDesc::SetDataLen(int len)
{
    dataLen = len;
}

void TcpMessageDesc::SetClient(Client * client)
{
    client_ = client;
}

int TcpMessageDesc::GetFd()
{
    return fd;
}

void TcpMessageDesc::SetFd(int fd_)
{
    fd = fd_;
}

Client* TcpMessageDesc::GetClient()
{
    return client_;
}

void Thread::DoUserWork(TcpMessageDescPtr& tcpPtr)
{
    func_(tcpPtr);
}

void Thread::SetWriteEvent(struct event_base* base, struct event* write_ev,
                    int fd, void* arg)
{
    event_set(write_ev, fd, EV_WRITE, Thread::OnWrite, arg);
    event_base_set(base, write_ev);
    event_add(write_ev, NULL);

    ((Client*)arg)->EventFlag |= WRITE_EVENT_ON;
}

void Thread::OnWrite(int fd, short event, void *arg)
{
    Client* client = (Client*)arg;
    Thread* thread = client->thread_;
    list<BufDescPtr>& list = client->sendBufList;

    client->EventFlag &= ~WRITE_EVENT_ON;

    while(list.size()){
        BufDescPtr bufPtr = list.front();
        int len = 0;
        len = send(fd, bufPtr->buf_, bufPtr->len_ - bufPtr->pos_, 0);
        
        //maybe some error happen
        if(len < 0){
            if(errno == EINTR || errno == EAGAIN){
                thread->SetWriteEvent(thread->base, client->pEvWrite, 
                                    fd,client);
                return ;
            }else{
                cout << "send data error happen" << endl;
                return;

            }
            
        }else{
            //one buf send ok
            if(len == (bufPtr->len_ - bufPtr->pos_)){
                list.pop_front();
            }else{//some data left
                bufPtr->pos_ += len + bufPtr->pos_;
                
                //add write event again,for next send
                Thread::SetWriteEvent(client->base, client->pEvWrite,fd, client);
            }

        }
    }
     
}

int Thread::Send(TcpMessageDescPtr tcpDesc, char* data, int len)
{
    BufDescPtr bufPtr;

    if(data == NULL || len <=0)
        return -1;

    bufPtr.reset(new BufDesc(data, len));

    Client *client = tcpDesc->GetClient();
    Thread *thread = client->thread_;


    client->sendBufList.push_back(bufPtr);

    //add the write event to libevent
    Thread::SetWriteEvent(tcpDesc->GetClient()->base, tcpDesc->GetClient()->pEvWrite,
                   tcpDesc->GetFd(), tcpDesc->GetClient());
    return 0;
}

void Thread::OnRead(int fd, short event, void *arg)
{
    struct Client * client = (struct Client*)arg;
    
    Thread* thread = client->thread_;
    cout << "read event has received" << endl;    
    //error process
    TcpMessageDescPtr tcpPtr(new TcpMessageDesc); 
    tcpPtr->SetFd(fd);
    tcpPtr->SetClient(client);
    
    char *buf = tcpPtr->GetBufPtr();
    int len = recv(fd, buf, MAXRECVLEN, 0); 
    cout << "read data from client, len is " << len <<endl;
    if(len <= 0) { 
        cout << "Client Close"<< endl; 

        //connection close(len=0) or error happen (len < 0) 
#if 0
        struct event *pEvRead = client->pEvRead;

        event_del(pEvRead); 
        free(pEvRead); 
#else
        //the clear work will be do in Client destruct process
        delete client;
#endif
        close(fd); 
        return; 
    }

    tcpPtr->SetDataLen(len);
    thread->DoUserWork(tcpPtr);    

    
}
    

void Thread::ProcessNewConnect(ConnInfo* info)
{
    int fd = info->fd;

    //set timeout for detect the connection lost 
    if(timeout_ > 0){
        int keepAlive = 1; // 开启keepalive属性
        int keepIdle = timeout_; // 如该连接在60秒内没有任何数据往来,则进行探测 
        int keepInterval = 5; // 探测时发包的时间间隔为5 秒
        int keepCount = 3; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.

        setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
        setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
        setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
        setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
    }
    
    {
        int flags;

        flags = fcntl(fd, F_GETFL);
        if (flags < 0)
            return ;
        flags |= O_NONBLOCK;
        if (fcntl(fd, F_SETFL, flags) < 0)
            return;

    }
    cout << "get fd from master thread, fd is " << fd << endl;
    struct Client * client = new Client(this, base);

    event_set(client->pEvRead, fd, EV_READ|EV_PERSIST, Thread::OnRead, client); 
    event_base_set(base, client->pEvRead); 
    event_add(client->pEvRead, NULL);
    client->EventFlag |= READ_EVENT_ON;
}


void Thread::OnAccept(int fd, short event, void *arg)
{
    char *buf[128];
    int len = 0;           
    Thread * thread = (Thread*)arg;

    do{
        len = read(fd, buf, sizeof(ConnInfo));
        
        if(len <= 0)
            return ;
        if((len != 0) && (len != sizeof(ConnInfo))){
            cout << "read conninfo error happen!" << endl;
            continue;
        }
        cout << "worker thread has read the conn info from pipe! len :  " << len <<endl;
        if(len > 0)
            thread->ProcessNewConnect( (ConnInfo*) buf);
    }while(len != 0);
}

void Thread::EventBaseDispatch()
{
    //event base init
    base = event_base_new();

    struct event evListen;

    event_set(&evListen, readfd_, EV_READ | EV_PERSIST, OnAccept, this);
    event_base_set(base, &evListen);
    event_add(&evListen, NULL);

    event_base_dispatch(base);
}

void* Thread::Process(void * param)
{
    Thread * thread = (Thread*) param;

    thread->EventBaseDispatch();
}

bool Thread::Start()
{
    if(pthread_create((pthread_t*)&tid_, NULL, Thread::Process, this) == 0){
        return true;    
    }
    
    return false;
}

bool Thread::Stop()
{
}

Thread::Thread(int read_fd, WorkFunction func, Uint16 timeout)
{
    func_ = func;
    readfd_ = read_fd;
    base = NULL;
    state_ = STATE_STOP;
    timeout_ = timeout;
    
}



Threadpool::Threadpool( Uint16 thread_num)
{
    thread_num_ = thread_num;
    next_ = 0;
}

bool Threadpool::Start(Uint16 port_, Uint16 timeout)
{
    //create and start threads
    if(!CreateThreads()){
        cout << "create threads error!" << endl;
        return false;
    }

    if(!StartThreads()){
        cout << "start threads error!" << endl;
        return false;
    }

    //create socket for listen and accept new connection
    int fd ;
    struct sockaddr_in addr;
    
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(port_);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        cout<<"create socket error" <<endl;
        return false;
    }
    
    if(bind(fd ,(struct sockaddr*) &addr, sizeof(addr)) == -1){
        cout << "bind error" << errno<<endl;
        close(fd);
        return false;
    }
    
    if(listen(fd, 20) == -1){
        cout << "listen error" <<errno<< endl;
        close(fd);
        return false;
    }
    
    //main thread loop
    while(1){
        struct sockaddr_in clientaddr;
        int    new_fd;
        int len = sizeof(clientaddr);

        if((new_fd = accept(fd, (struct sockaddr*)&clientaddr,(socklen_t*) &len)) == -1){
            cout << "accept error" << errno<<endl;
        }
        //dispatch the new connection to threads 
        ThreadInfo * selectedThread = NULL;
    
        selectedThread = &threads_[next_];
        
        struct ConnInfo connInfo;
        connInfo.fd = new_fd;
        memcpy(&connInfo.addr , &clientaddr, sizeof(connInfo.addr));
        cout << "accept one connection , fd is " << new_fd;
        write(selectedThread->writefd_, &connInfo, sizeof(connInfo)); 
        if(next_ == thread_num_){
            next_ = 0;
        }else{
            next_ ++;
        }    
    }
}

void Threadpool::RegistersJob(WorkFunction job)
{
    job_ = job;
}

bool Threadpool::Stop()
{
}

bool Threadpool::CreateThreads()
{
    for(int i = 0; i < thread_num_; i++){
        //create pipe for thread  communicate
        int ret = -1;
        int fd[2];
        
        ret = pipe(fd);
        if(ret < 0){
            perror("pipe create fail!");
            return false;
        }
        {
            int flags;

            flags = fcntl(fd[0], F_GETFL);
            if (flags < 0)
                return false;
            flags |= O_NONBLOCK;
            if (fcntl(fd[0], F_SETFL, flags) < 0)
                return false;

        }
        //create Thread and init
        ThreadPtr threadPtr(new Thread(fd[0], job_));

        ThreadInfo info;
        info.thread_ = threadPtr;
        info.writefd_ = fd[1];

        threads_.push_back(info);
    }

    return true;
}

bool Threadpool::StartThreads()
{
    for(int i=0; i < threads_.size(); i++){
        if(!threads_[i].thread_->Start()){
            cout << "start thread " << i << " error" << endl;
            return false;
        }
    }
    return true;
}
