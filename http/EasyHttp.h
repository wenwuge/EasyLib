#ifndef __EASY_HTTP
#define __EASY_HTTP
#include "HttpService.h"
#include "../thread/EventLoop.h"
#include "../thread/EventLoopThreadPool.h"
using namespace std;

class HttpServer{
    typedef tr1::shared_ptr<HttpService> HttpServicePtr;
    typedef tr1::shared_ptr<EventLoop> EventLoopPtr;
    typedef tr1::shared_ptr<event_base> EvetnBasePtr;
    typedef HttpService::ResponseCallBack ResponseCallBack;
    typedef tr1::function<void(HttpContextPtr& req, ResponseCallBack callback)> Processor; 
    typedef tr1::shared_ptr<Condition> ConditionPtr;
    typedef tr1::shared_ptr<EventLoopThread> EventLoopThreadPtr;
public:
    HttpServer(int thread_num);
    void Init();
    void Listen(uint16_t port);
    void Start();
    void Stop(bool wait_until_stop=true);
    void RegisterUrlProcessor(string& url, Processor processor);

private:
    void DispatchRequests(HttpContextPtr& req, ResponseCallBack callback);
    void RunProcessor(HttpContextPtr& req, ResponseCallBack callback, Processor& processor);
    void StopInLoop();
private:
    HttpServicePtr http_server_;
    //for master threads use
    tr1::shared_ptr<EventLoopThreadPool> thread_pool_;
#if 0
    EventLoopPtr  event_loop_;
#else
    EventLoopThreadPtr event_loop_thread_;
#endif
private:
    int worker_thread_num_;
    map<string, Processor> url_processors_;
    ConditionPtr stop_condition_;
    bool stop_;

};
#endif
