#ifndef __CONNECTION
#define __CONNECTION
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef boost::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef boost::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;

typedef boost::function<void (const TcpConnectionPtr&,
                              Buffer*,
                              Timestamp)> MessageCallback;
class TcpConnection{
public:
    TcpConnection(EventLoopThread * thread, int fd);
    void Send(void * data, uint32_t len);
    void Shutdown();
    void ForceClose();
    void ConnectionEstablished(); 
    //functions below, set the callback from users 
    void SetMessageRecvCallback(MessageCallback &callback){
        message_callback_ =  callback;
    }
    void SetWriteCompletedCallback(WriteCompleteCallback &callback){
        writecomplete_callback_ = callback;
    }
    void SetConnectionCloseCallback(ConnectionCallback callback){
        closed_callback_ = callback;
    }
    void SetConnectionEstablishedCallback(ConnectionCallback callback){
        established_callback_ = callback;
    }
private:
    //functions below ,the channel will callback
    void HandleRead(Timestamp ts);
    void HandleWrite();
    void HandleClose();


private:
    boost::scoped_ptr<Channel> channel_;
    EventLoopThread * thread_;
    int fd_;
    Buffer send_buffer_;
    Buffer recv_buffer_;

    //various callbacks
    MessageCallback message_callback_;
    WriteCompleteCallback writecomplete_callback_;
    ConnectionCallback   established_callback_;
    ConnectionCallback   closed_callback_;


};
#endif
