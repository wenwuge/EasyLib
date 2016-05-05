#ifndef __TCPSERVER
#define __TCPSERVER
#include <Thread.h>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <channel.h>
#include <vector>
#include <eventloopthread.h>
#include <netinet/in.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <actor.h>

using namespace muduo;
using namespace std;
typedef boost::shared_ptr<Channel> ConnPtr;

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
    int Start();
    void Stop();

private:
    void HandleReadEvent(Timestamp ts);
    void NewConnectionEstablished(int fd,struct sockaddr_in &peer); 
private:
    Options options_;
    vector<EventLoopThread> threads_;
    boost::scoped_ptr<Actor> actor_; //master thread's loop
    boost::scoped_ptr<Channel> accept_channel_;
    uint8_t state_;
    int listen_fd_;
    int next_id_;

};

#endif
