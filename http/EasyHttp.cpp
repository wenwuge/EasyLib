#include "EasyHttp.h"

HttpServer::HttpServer(int thread_num):stop_(true)
{
    worker_thread_num_ = thread_num;
}

void HttpServer::Init()
{
    //init worker thread pool
#if 0
    event_loop_.reset(new EventLoop);
#else
    event_loop_thread_.reset(new EventLoopThread);
#endif
    thread_pool_.reset(new EventLoopThreadPool(event_loop_thread_->GetEventLoop().get()));
    thread_pool_->SetThreadNum(worker_thread_num_);

    //init master thread
    http_server_.reset(new HttpService(event_loop_thread_->GetEventBase()));

    //register the url callback
    http_server_->RegisterDefaultCallback(std::tr1::bind(&HttpServer::DispatchRequests, this, tr1::placeholders::_1, tr1::placeholders::_2));

    //init the stop condtion
    stop_condition_.reset(new Condition(event_loop_thread_->GetEventBase()));
    stop_condition_->Init();
    stop_condition_->Watch(tr1::bind(&HttpServer::StopInLoop, this) ,0);
}

void HttpServer::Listen(uint16_t port)
{
    http_server_->Listen(port);
}

void HttpServer::Start()
{
    thread_pool_->Start(true);
#if 0
    event_loop_->Run();
#else
    event_loop_thread_->Start(true);
    stop_ = false;
#endif

}

void HttpServer::Stop(bool wait_until_stop)
{
    stop_condition_->Notify();   
    while(!stop_){
        usleep(10000);
    }
}

//stop httpserver in main loop
void HttpServer::StopInLoop()
{
    http_server_->Stop();
    thread_pool_->Stop(true);
    //run the pending functors in the queue
    event_loop_thread_->Stop(false);
    stop_ = true;
}

void HttpServer::RunProcessor(HttpContextPtr& req, ResponseCallBack callback, Processor& processor)
{
    processor(req, callback);
}

void HttpServer::DispatchRequests(HttpContextPtr& req, ResponseCallBack callback)
{
    //select the loop and call the url processor
    EventLoop * loop = NULL;

    loop = thread_pool_->GetNextLoop();
    

    map<string, Processor>::iterator iter;
    iter = url_processors_.find(req->GetUrl());
    if(iter != url_processors_.end()){
        loop->RunInLoop(tr1::bind(&HttpServer::RunProcessor, this, req, callback, iter->second));
    }else{
        string response = "invalid url";
        callback(response);
    }

}

void HttpServer::RegisterUrlProcessor(string& url, Processor  processor)
{
    url_processors_[url] = processor;
}
