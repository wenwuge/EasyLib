// event_loop_test.cc (2014-07-16)
// Yan Gaofeng (yangaofeng@360.cn)

#include <ldd/net/event_loop.h>
#include <ldd/net/event_loop_thread_pool.h>

#include <gtest/gtest.h>
#include <glog/logging.h>

#include <boost/bind.hpp>

using namespace ldd::net;

class MyEventLoop {
public:
    MyEventLoop() : ok_(false) {}
    ~MyEventLoop() {}

    bool Init();
    void Finalize();

    bool RunInLoop();
    bool QueueInLoop();

private:
    void Func(int no);

private:
    EventLoop loop_;

    bool ok_;
};

void MyEventLoop::Func(int no)
{
    LOG(INFO) << "func run, no: " << no;
    ok_ = true;
    loop_.Stop();
}

bool MyEventLoop::Init()
{
    LOG(INFO) << "Enter init";
    return true;
}

void MyEventLoop::Finalize()
{
    LOG(INFO) << "Enter Finalize";
}

bool MyEventLoop::RunInLoop()
{
    loop_.RunInLoop(boost::bind(&MyEventLoop::Func, this, 1));

    loop_.Run();

    if (!ok_) {
        return false;
    }

    ok_ = false;

    return true;
}

bool MyEventLoop::QueueInLoop()
{
    loop_.QueueInLoop(boost::bind(&MyEventLoop::Func, this, 2));

    loop_.Run();

    if (!ok_) {
        return false;
    }

    ok_ = false;

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

class MyEventLoopTest : public testing::Test {
public:
    virtual void SetUp();
    virtual void TearDown();

protected:
    MyEventLoop loop_;
};

void MyEventLoopTest::SetUp()
{
    loop_.Init();
}

void MyEventLoopTest::TearDown()
{
    loop_.Finalize();
}


TEST_F(MyEventLoopTest, EventLoopTest)
{
    ASSERT_TRUE(loop_.RunInLoop());
    ASSERT_TRUE(loop_.QueueInLoop());
}

////////////////////////////////////////////////////////////////////////////////////

class MyEventLoopThread {
public:
    MyEventLoopThread() : ok_(false) {}
    ~MyEventLoopThread() {}

    bool Init();
    void Finalize();

    bool RunInLoop();
    bool QueueInLoop();

private:
    void Func(int no);

private:
    EventLoopThread loop_thread_;

    bool ok_;
};

void MyEventLoopThread::Func(int no)
{
    LOG(INFO) << "func run, no: " << no;
    ok_ = true;
}

bool MyEventLoopThread::Init()
{
    LOG(INFO) << "Enter init";
    loop_thread_.Start();
    return true;
}

void MyEventLoopThread::Finalize()
{
    LOG(INFO) << "Enter Finalize";

    loop_thread_.Stop();
    loop_thread_.Join();
}

bool MyEventLoopThread::RunInLoop()
{
    loop_thread_.event_loop()->RunInLoop(boost::bind(&MyEventLoopThread::Func, this, 1));

    usleep(100*1000);

    if (!ok_) {
        return false;
    }

    ok_ = false;

    return true;
}

bool MyEventLoopThread::QueueInLoop()
{
    loop_thread_.event_loop()->QueueInLoop(boost::bind(&MyEventLoopThread::Func, this, 2));

    usleep(100*1000);

    if (!ok_) {
        return false;
    }

    ok_ = false;

    return true;
}

//////////////////////////////////////////////////////////////////////////////

class MyEventLoopThreadTest : public testing::Test {
public:
    virtual void SetUp();
    virtual void TearDown();

protected:
    MyEventLoopThread loop_;
};

void MyEventLoopThreadTest::SetUp()
{
    loop_.Init();
}

void MyEventLoopThreadTest::TearDown()
{
    loop_.Finalize();
}


TEST_F(MyEventLoopThreadTest, EventLoopThreadTest)
{
    ASSERT_TRUE(loop_.RunInLoop());
    ASSERT_TRUE(loop_.QueueInLoop());
}


////////////////////////////////////////////////////////////////////////////////

#define THREAD_COUNT 10

class MyEventLoopThreadPool {
public:
    MyEventLoopThreadPool() : loop_thread_pool_(&loop_) {}
    ~MyEventLoopThreadPool() {}

    bool Init();
    void Finalize();

    bool RunInLoop();
    bool QueueInLoop();

private:
    void Func(int no);

private:
    EventLoop loop_;
    EventLoopThreadPool loop_thread_pool_;

    bool ok_[THREAD_COUNT];
};

void MyEventLoopThreadPool::Func(int no)
{
    LOG(INFO) << "func run, no: " << no;
    ok_[no] = true;
}

bool MyEventLoopThreadPool::Init()
{
    LOG(INFO) << "Enter init";
    for (int i = 0; i < THREAD_COUNT; i++) {
        ok_[i] = false;
    }

    loop_thread_pool_.SetThreadNum(THREAD_COUNT);
    if (!loop_thread_pool_.Start()) {
        LOG(ERROR) << "thread pool start failed";
        return false;
    }

    return true;
}

void MyEventLoopThreadPool::Finalize()
{
    LOG(INFO) << "Enter Finalize";

    loop_thread_pool_.Stop();
    //loop_thread_pool_.Join();
}

bool MyEventLoopThreadPool::RunInLoop()
{
    for (int i = 0; i < THREAD_COUNT; i++) {
        loop_thread_pool_.GetNextLoop()->RunInLoop(
                boost::bind(&MyEventLoopThreadPool::Func, this, i));
    }

    usleep(200*1000);

    bool ok = true;
    for (int i = 0; i < THREAD_COUNT; i++) {
        if (!ok_[i]) {
            ok = false;
        }

        ok_[i] = false;
    }

    return ok;
}

bool MyEventLoopThreadPool::QueueInLoop()
{
    for (int i = 0; i < THREAD_COUNT; i++) {
        loop_thread_pool_.GetNextLoop()->QueueInLoop(
                boost::bind(&MyEventLoopThreadPool::Func, this, i));
    }

    usleep(200*1000);

    bool ok = true;
    for (int i = 0; i < THREAD_COUNT; i++) {
        if (!ok_[i]) {
            ok = false;
        }

        ok_[i] = false;
    }

    return ok;
}

//////////////////////////////////////////////////////////////////////////////

class MyEventLoopThreadPoolTest : public testing::Test {
public:
    virtual void SetUp();
    virtual void TearDown();

protected:
    MyEventLoopThreadPool loop_;
};

void MyEventLoopThreadPoolTest::SetUp()
{
    loop_.Init();
}

void MyEventLoopThreadPoolTest::TearDown()
{
    loop_.Finalize();
}


TEST_F(MyEventLoopThreadPoolTest, EventLoopThreadTest)
{
    ASSERT_TRUE(loop_.RunInLoop());
    ASSERT_TRUE(loop_.QueueInLoop());
}

