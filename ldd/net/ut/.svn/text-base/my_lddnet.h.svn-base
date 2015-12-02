// my_lddnet.h (2014-07-17)
// Yan Gaofeng (yangaofeng@360.cn)

#include <ldd/net/client.h>
#include <ldd/net/server.h>
#include <ldd/net/event_loop.h>
#include <ldd/net/listener.h>

extern const std::string REQUEST;
extern const std::string EXTRAS;

class MyLddNet {
public:
    MyLddNet() : listener_(&loop_), ok_(true) {}
    ~MyLddNet() {}

    bool Init();
    void Finalize();

    bool Echo();

    void set_ok(bool ok) { ok_ = ok; }

    ldd::net::EventLoop *event_loop() { return &loop_; }

private:
    ldd::net::EventLoop loop_;
    ldd::net::Listener listener_;
    boost::scoped_ptr<ldd::net::Server> server_;
    boost::scoped_ptr<ldd::net::Client> client_;

    bool ok_;
};

