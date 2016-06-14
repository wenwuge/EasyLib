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
#include "connection.h"
#include "buffer.h"
#include "workers.h"

using namespace muduo;
using namespace std;
typedef boost::shared_ptr<Channel> ConnPtr;

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef boost::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef boost::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;

typedef boost::function<void (const TcpConnectionPtr&,
                              Buffer*,
                              Timestamp)> MessageCallback;
typedef boost::function<void ()> CpuCostJob;

struct Options{
    uint16_t thread_num_;
    uint16_t port_;
    string addr_;
    int   workers_num_;

    MessageCallback  rev_cb_;
    WriteCompleteCallback     send_cb_;
    ConnectionCallback connected_cb_;
    ConnectionCallback close_cb_;
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
    void QueueCpuCostJob(const CpuCostJob& job);

private:
    void HandleReadEvent(Timestamp ts);
    void NewConnectionEstablished(int fd,struct sockaddr_in &peer); 
    void RemoveTcpConnection(const TcpConnectionPtr& conn);
    void RemoveTcpConnectionInLoop(const TcpConnectionPtr& conn);
private:
    Options options_;
    vector<EventLoopThread*> threads_;
    boost::scoped_ptr<Actor> actor_; //master thread's loop
    boost::scoped_ptr<Channel> accept_channel_;
    uint8_t state_;
    int listen_fd_;
    int next_id_;

    //various callbacks
    #if 0
    MessageCallback message_callback_;
    WriteCompleteCallback writecomplete_callback_;
    ConnectionCallback   established_callback_;
    ConnectionCallback   closed_callback_;
    #endif
    map<int, TcpConnectionPtr> conns_;
    boost::scoped_ptr<WorkerManager> worker_manager_;
};

#endif
