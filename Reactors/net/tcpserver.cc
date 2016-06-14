#include "tcpserver.h"
#include<stdio.h>
#include<stdlib.h>
#include <arpa/inet.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <signal.h>

#define BACKLOG 20
TcpServer::TcpServer():actor_(new Actor()),
        next_id_(0)
{
}

void TcpServer::SetOptions(Options& option)
{
    options_ = option;
}

int TcpServer::Start()
{
    signal(SIGPIPE, SIG_IGN);
    for(int i = 0; i < options_.thread_num_; i++){
        string name = "thread";
        threads_.push_back(new EventLoopThread(name));
    }
    //start background threads
    for(int i = 0; i < threads_.size(); i++)
        threads_[i]->Run();

    //add listen socket into local event loop
    int listenfd;
    struct sockaddr_in server;
    if((listenfd = socket(AF_INET, SOCK_STREAM| SOCK_NONBLOCK | SOCK_CLOEXEC, 0)) == -1){
        perror("create socket failed");
        return -1;
    }

    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
            &opt, sizeof(opt));
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(options_.port_);
    server.sin_addr.s_addr = inet_addr(options_.addr_.c_str());

    if(bind(listenfd, (struct sockaddr*)&server, sizeof(server)) == -1){
        perror("binderror");
        return -1;
    }

    if(listen(listenfd, BACKLOG) == -1){
        perror("listenerror");
        return -1;
    }
    listen_fd_ = listenfd;
    accept_channel_.reset(new Channel(actor_.get(), listenfd));   
    
    //set accept channel callback
    accept_channel_->setReadCallback(
            boost::bind(&TcpServer::HandleReadEvent, this, _1));
    accept_channel_->enableReading();
    //accept_channel_->setCloseCallBack();
    //accept_channel_->setErrorCallBack();
    //actor_->UpdateChannel(accept_channel_.get());
    //init worker manager
    if(options_.workers_num_ > 0){
       worker_manager_.reset(new WorkerManager(options_.workers_num_)); 
       worker_manager_->StartWork();
    }

    state_ = RUNNING;
    //start master evnet loop
    actor_->Loop();
}

void TcpServer::HandleReadEvent(Timestamp ts)
{
   struct sockaddr_in client;
   socklen_t addrlen;

   addrlen = sizeof(client);
   int connfd = accept4(listen_fd_, (struct sockaddr*)&client, &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
   if(connfd >= 0){
        NewConnectionEstablished(connfd, client);
   }else{
       //error process
        if(errno != EAGAIN){
            cout << " accept error, errno : " << errno << endl;
            close(connfd);
        }

   }
}
void TcpServer::RemoveTcpConnection(const TcpConnectionPtr& conn)
{
    actor_->QueueInActor(boost::bind(&TcpServer::RemoveTcpConnectionInLoop,
                this, conn));    
}
void TcpServer::RemoveTcpConnectionInLoop(const TcpConnectionPtr& conn)
{
    conns_.erase(conn->fd());
    
    options_.close_cb_(conn);
}

void TcpServer::NewConnectionEstablished(int fd,struct sockaddr_in &peer) 
{
    cout << "remote ip: " << inet_ntoa(peer.sin_addr) << " port: " << ntohs(peer.sin_port) << endl; 
    //select one event_loop_thread
    EventLoopThread * selected_loop = threads_[next_id_];
    if(next_id_ == (options_.thread_num_ - 1)){
        next_id_ = 0;
    }else{
        next_id_ ++ ;
    }

    //create connection object
    boost::shared_ptr<TcpConnection> conn_ptr(new TcpConnection(selected_loop,fd));  

    //set write,read,close callbacks
    conn_ptr->SetMessageRecvCallback(options_.rev_cb_);
    conn_ptr->SetWriteCompletedCallback(options_.send_cb_);
    conn_ptr->SetConnectionDisconnectedCallback(options_.close_cb_);
    conn_ptr->SetConnectionCloseCallback(boost::bind(&TcpServer::RemoveTcpConnection,
                this, _1));
    conn_ptr->SetConnectionEstablishedCallback(options_.connected_cb_);
    
    conns_[fd] = conn_ptr;
    //put the functor into the loop selected 's functor queue
    selected_loop->QueueInLoop(boost::bind(&TcpConnection::ConnectionEstablished, conn_ptr));
    //selected_loop->QueueInLoop(boost::bind(&TcpServer::Stop, this));
}
void TcpServer::Stop()
{
}

void TcpServer::QueueCpuCostJob(const CpuCostJob& job)
{
    worker_manager_->QueueJob(job);
}
