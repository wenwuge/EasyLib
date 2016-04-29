#ifdef __TCPSERVER
#define __TCPSERVER
#include <Thread.h>
using namespace muduo;
using namespace std;
typedef boost::shared_ptr<Connection> ConnPtr;

typedef boost::function<void(ConnPtr)> ReceiveCallback;
typedef boost::function<void(ConnPtr)> SendCallback;
typedef boost::function<void(int)> OnConnectedCallback;
typedef boost::function<void(ConnPtr)> OnClosedCallback;

struct Options{
    uint16_t thread_num_;
    uint16_t port_;
    string addr_;

    ReceiveCallback rev_cb_;
    SendCallback    send_cb_;
    OnConnectedCallback connected_cb_;
    OnClosedCallback close_cb_;
};

class TcpServer{
public:
    enum{
        STOPPED = 0,
        RUNNING = 1,
        STOPPING = 2
    };
    TcpServer();
    
    //set thread num, port, callbacks
    void SetOptions(Options& option);
    void Start();
    void Stop();

private:
    void HandleReadEvent();
    void NewConnectionEstablished(int fd,struct addr_in &peer); 
private:
    Options options_;
    vector<EventLoopThread> threads_;
    boost::scoped_ptr<Actor> actor_; //master thread's loop
    boost::scoped_ptr<Channel> accept_channel_;
    uint8_t state_;
    int listen_fd_;

};

void TcpServer::SetOptions(Options& option)
{
    options_ = option;
}

int TcpServer::Start()
{
    //start background threads
    for(int i = 0; i < threads_.size(); i++)
        threads_[i].Run();

    //add listen socket into local event loop
    int listenfd;
    struct sockaddr_in server;
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("create socket failed");
        return -1;
    }

    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
            &opt, sizeof(opt));
    bzero(&server, sizeof(serer));
    server.sin_family = AF_INET;
    server.sin_port = htons(options_.port_);
    server.sin_addr.s_addr = inet_addr(options_.addr_.c_str()):

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
    accept_channel_->setReadCallBack(
            boost::bind(&TcpServer::HandleReadEvent, this);
    //accept_channel_->setCloseCallBack();
    //accept_channel_->setErrorCallBack();
    actor_->UpdateChannel(accept_channel_.get());
    

    state_ = RUNNING;
    //start master evnet loop
    actor_->Loop();
}

void TcpServer::HandleReadEvent()
{
   struct sockaddr_in client;
   socklen_t addrlen;

   addrlen = sizeof(client);
   int connfd = accept(listen_fd_, &client, &addrlen);
   if(connfd >= 0){
        NewConnectionEstablished(connfd, client)
   }else{
       //error process
   }
}

void TcpServer::NewConnectionEstablished(int fd,struct addr_in &peer) 
{
}
void TcpServer::Stop()
{
}
#endif
