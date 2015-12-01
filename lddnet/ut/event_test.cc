// event_test.cc (2014-07-14)
// Yan Gaofeng (yangaofeng@360.cn)

#include <string.h>
#include <errno.h>
#include <pthread.h>

#include <boost/bind.hpp>

#include <ldd/net/event.h>
#include <ldd/net/event_loop.h>

#include <gtest/gtest.h>
#include <glog/logging.h>

#include "x_socket.h"

using namespace ldd::util;
using namespace ldd::net;

class TCPServer {
public:
    TCPServer() : port_(0), fd_(-1), tid_(0), quit_(false) {}
    ~TCPServer() {}

    bool Init(int port);

    bool Start();
    void Stop();

private:
    void RealWork();
    static void *work_func(void *arg);

private:
    int port_;
    int fd_;

    pthread_t tid_;
    bool quit_;
};

bool TCPServer::Init(int port)
{
    int s = x_create_tcp_server("0.0.0.0", port);
    if (s == -1) {
        LOG(ERROR) << "create tcp server failed";
        return false;
    }

    fd_ = s;

    return true;
}

bool TCPServer::Start()
{
    int rc = pthread_create(&tid_, NULL, TCPServer::work_func, this);
    if (rc == -1) {
        LOG(ERROR) << "create thread error: " << strerror(errno);
        return false;
    }

    usleep(10*1000);

    return true;
}

void TCPServer::Stop()
{
    quit_ = true;

    close(fd_);

    int rc = pthread_join(tid_, NULL);
    if (rc == -1) {
        LOG(ERROR) << "pthread join error: " << strerror(errno);
        return;
    }

    LOG(INFO) << "server thread exit";
}

void *TCPServer::work_func(void *arg)
{
    CHECK_NOTNULL(arg);

    TCPServer *self = (TCPServer *)arg;

    self->RealWork();

    return NULL;
}


void TCPServer::RealWork()
{
    while (!quit_) {
        char ip[32];
        int port;
        int cli_fd = x_accept(fd_, ip, &port);
        if (cli_fd == -1) {
            LOG(ERROR) << "accept client failed";
            break;
        }

        LOG(INFO) << "accept a client: " << ip << ":" << port;

        while (!quit_) {
            char buf[1024];
            int bytes = x_recvbytes(cli_fd, buf, 1024);
            if (bytes <= 0) {
                LOG(ERROR) << "recv error: " << strerror(errno);
                close(cli_fd);
                break;
            }

            buf[bytes] = '\0';

            LOG(INFO) << "recv a msg: " << buf;

            int rc = x_send(cli_fd, buf, bytes);
            if (rc == -1) {
                LOG(ERROR) << "send error: " << strerror(errno);
                close(cli_fd);
                break;
            }

            LOG(INFO) << "echo a msg to client, msg len: " << bytes;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

class TCPClient {
public:
    TCPClient(EventLoop *event_loop) 
        : fd_(0), 
        event_loop_(event_loop),
        ev_(event_loop), 
        ok_(false) {}

    ~TCPClient() { Close(); }

    bool Init(int port);

    void Echo(const std::string &msg);

    bool ok() { return ok_; }

    void Close();

private:
    void OnWrite(int events);
    void OnRead(int events);

private:
    int fd_;

    EventLoop *event_loop_;
    FdEvent ev_;

    std::string msg_;

    bool ok_;
};

bool TCPClient::Init(int port)
{
    fd_ = x_create_tcp_client("0.0.0.0", port);
    if (fd_ == -1) {
        LOG(ERROR) << "create tcp client error: " << strerror(errno);
        return false;
    }

    int rc = evutil_make_socket_nonblocking(fd_);
    if (rc != 0) {
        LOG(ERROR) << "make socket to noblocking failed, fd: " << fd_;
        return false;
    }

    return true;
}

void TCPClient::Echo(const std::string &msg)
{
    msg_ = msg;

    TimeDiff td = TimeDiff::Milliseconds(50);
    ev_.AsyncWait(fd_, 
            FdEvent::kWritable, 
            boost::bind(&TCPClient::OnWrite, this, _1)
            , td);
}

void TCPClient::OnWrite(int events)
{
    if (!(events & FdEvent::kWritable)) {
        LOG(ERROR) << "write events timeout";
        return;
    }

    int rc = x_send(fd_, const_cast<char *>(msg_.c_str()), msg_.size());
    if (rc == -1) {
        LOG(ERROR) << "send msg failed, msg: " << msg_;
        return;
    }

    LOG(INFO) << "send a msg: " << msg_;

    TimeDiff td = TimeDiff::Milliseconds(500);
    ev_.AsyncWait(fd_, 
            FdEvent::kReadable, 
            boost::bind(&TCPClient::OnRead, this, _1), 
            td);
}

void TCPClient::OnRead(int events)
{
    LOG(INFO) << "Enter OnRead";

    do {
        if (!(events & FdEvent::kReadable)) {
            LOG(ERROR) << "read events timeout, events: " << events;
            break;
        }

        char buf[1024] = {0};
        LOG(INFO) << "read fd: " << fd_;
        int rc = x_recvbytes(fd_, buf, 1024);
        if (rc == -1) {
            LOG(ERROR) << "recv msg failed";
            break;
        }

        if (msg_ == buf) {
            ok_ = true;
        } else {
            ok_ = false;
        }
    } while (0);

    event_loop_->Stop();
}

void TCPClient::Close()
{
    if (fd_ != -1) {
        close(fd_);
        fd_ = -1;
    }
}

///////////////////////////////////////////////////////////////////////////////

class MyFdEvent {
public:
    MyFdEvent() : client_(&loop_) {}
    ~MyFdEvent() {}

    bool Init();
    void Finalize();

    bool Echo(const std::string &msg);

private:
    EventLoop loop_;
    TCPServer server_;
    TCPClient client_;
};

bool MyFdEvent::Init()
{
    bool ret = server_.Init(60000);
    if (!ret) {
        LOG(ERROR) << "init server failed";
        return false;
    }

    if (!server_.Start()) {
        LOG(ERROR) << "start server failed";
        return false;
    }

    ret = client_.Init(60000);
    if (!ret) {
        LOG(ERROR) << "init client failed";
        return false;
    }

    return true;
}

void MyFdEvent::Finalize()
{
    server_.Stop();
    client_.Close();
}

bool MyFdEvent::Echo(const std::string &msg)
{
    client_.Echo(msg);
    loop_.Run();
    client_.Close();

    return client_.ok();
}
///////////////////////////////////////////////////////////////////////////////

class MyTimerEvent {
public:
    MyTimerEvent() : ok_(false)
                     , timer_(&event_loop_)
                     , wait_ms_(0) {}
    ~MyTimerEvent() {}

    bool Init();
    void Finalize();

    bool Wait(uint64_t wait_ms);

private:
    void OnExpire();
    void OnWait();

private:
    bool ok_;
    Timestamp t1_;
    Timestamp t2_;

    EventLoop event_loop_;
    TimerEvent timer_;
    uint64_t wait_ms_;
};

bool MyTimerEvent::Init()
{
    LOG(INFO) << "Enter MyTimerEvent::Init";

    return true;
}

void MyTimerEvent::Finalize()
{
    LOG(INFO) << "Enter MyTimerEvent::Finalize";
}

bool MyTimerEvent::Wait(uint64_t wait_ms)
{
    wait_ms_ = wait_ms;
    t1_ = Timestamp::Now();
    timer_.AsyncWait(boost::bind(&MyTimerEvent::OnWait, this), 
                TimeDiff::Milliseconds(wait_ms));

    event_loop_.Run();

    return ok_;
}

void MyTimerEvent::OnWait()
{
    t2_ = Timestamp::Now();

    LOG(INFO) << "Enter OnWait";

    TimeDiff td = t2_ - t1_;

    uint64_t used_time = td.ToMilliseconds();
    if (used_time < wait_ms_ || used_time > wait_ms_ + 10) {
        LOG(ERROR) << "wait timer error";
        ok_ = false;
        event_loop_.Stop();
        return;
    }

    LOG(INFO) << "timer used time: " << used_time << "ms";

    //shoud in OnExpire, but expire have problem
    ok_ = true;
    event_loop_.Stop();

    return;

#if 0
    Timestamp t = Timestamp::Now();
    t += TimeDiff::Milliseconds(100);

    //test expire
    timer_.ExpiresAt(&t);
#endif
}

///////////////////////////////////////////////////////////////////////////////

class MyFdEventTest : public testing::Test {
public:
    virtual void SetUp();
    virtual void TearDown();

protected:
    MyFdEvent my_event_;
};

void MyFdEventTest::SetUp()
{
    my_event_.Init();
}

void MyFdEventTest::TearDown()
{
    my_event_.Finalize();
}


TEST_F(MyFdEventTest, FDEventTest)
{
    ASSERT_TRUE(my_event_.Echo("Hello world"));
}

///////////////////////////////////////////////////////////////////////////////

class MyTimerEventTest : public testing::Test {
public:
    virtual void SetUp();
    virtual void TearDown();

protected:
    MyTimerEvent my_event_;
};

void MyTimerEventTest::SetUp()
{
    my_event_.Init();
}

void MyTimerEventTest::TearDown()
{
    my_event_.Finalize();
}


TEST_F(MyTimerEventTest, TimerEventTest)
{
    //in ms
    ASSERT_TRUE(my_event_.Wait(100));
}

///////////////////////////////////////////////////////////////////////////////

class MySignalEvent {
public:
    MySignalEvent() : sig_ev_(&loop_), ok_(false) {}
    ~MySignalEvent() {}

    bool Init();
    void Finalize();

    bool WaitSignal();
    bool RemoveSignal();
    bool SignalTimeout();

private:
    void SigHandler(int sig);

private:
    EventLoop loop_;
    SignalEvent sig_ev_;

    bool ok_;
};

bool MySignalEvent::Init()
{
    LOG(INFO) << "Enter MySignalEvent::Init";
    return true;
}

void MySignalEvent::Finalize()
{
    LOG(INFO) << "Enter MySignalEvent::Finalize";
}

bool MySignalEvent::WaitSignal()
{
    bool ret = sig_ev_.Add(SIGUSR1);
    if (!ret) {
        LOG(ERROR) << "add signal SIGUSR1 failed";
        return false;
    }

    sig_ev_.AsyncWait(boost::bind(&MySignalEvent::SigHandler, this, _1));

    pid_t pid = getpid();
    LOG(INFO) << "Wait signal, pid: " << pid;

    int rc = kill(pid, SIGUSR1);
    if (rc == -1) {
        LOG(ERROR) << "send signal error: " << strerror(errno);
        return false;
    }

    loop_.Run();


    if (!ok_) {
        return false;
    }

    ok_ = false;

    ret = sig_ev_.Remove(SIGUSR1);
    if (!ret) {
        LOG(ERROR) << "remove SIGUSR1 failed";
        return false;
    }

    return true;
}

bool MySignalEvent::RemoveSignal()
{
    bool ret = sig_ev_.Add(SIGUSR1);
    if (!ret) {
        return false;
    }

    ret = sig_ev_.Remove(SIGUSR1);
    if (!ret) {
        LOG(ERROR) << "remove sig SIGUSR1 failed";
        return false;
    }

#if 0
    pid_t pid = getpid();
    int rc = kill(pid, SIGUSR1);
    if (rc == -1) {
        LOG(ERROR) << "send signal SIGUSR1 error: " << strerror(errno);
        return false;
    }

    loop_.Run();

    sig_ev_.Remove(SIGUSR1);

    if (!ok_) {
        return false;
    }
#endif

    return true;
}

bool MySignalEvent::SignalTimeout()
{
    bool ok = sig_ev_.Add(SIGUSR1);
    if (!ok) {
        return false;
    }

    sig_ev_.AsyncWait(boost::bind(&MySignalEvent::SigHandler, this, _1),
            TimeDiff::Milliseconds(100));

    usleep(110);

    loop_.Run();

    if (!ok_) {
        LOG(ERROR) << "signal timeout error";
        return false;
    }

    ok_ = false;

#if 0
    //not needed, because it calls Clear method after timeout
    ok = sig_ev_.Remove(SIGUSR1);
    if (!ok) {
        LOG(ERROR) << "remove SIGUSR1 failed in SignalTimeout func";
        return false;
    }
#endif

    return true;
}

void MySignalEvent::SigHandler(int sig)
{
    switch (sig) {
    case 0:
        LOG(INFO) << "signal timeout ok";
        ok_ = true;
        break;
    case SIGUSR1:
        LOG(INFO) << "recv SIGUSR1";
        ok_ = true;
        break;
    default:
        LOG(ERROR) << "unregistered signal: " << sig;
        ok_ = false;
        break;
    }

    loop_.Stop();
}

/////////////////////////////////////////////////////////////////////////

class MySignalEventTest : public testing::Test {
public:
    virtual void SetUp();
    virtual void TearDown();

protected:
    MySignalEvent my_event_;
};

void MySignalEventTest::SetUp()
{
    my_event_.Init();
}

void MySignalEventTest::TearDown()
{
    my_event_.Finalize();
}


TEST_F(MySignalEventTest, SignalEventTest)
{
    ASSERT_TRUE(my_event_.RemoveSignal());
    ASSERT_TRUE(my_event_.WaitSignal());
    ASSERT_TRUE(my_event_.SignalTimeout());
}

