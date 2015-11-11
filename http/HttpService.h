#ifndef __HTTP_SERVICE
#define __HTTP_SERVICE
#include <unistd.h>
#include <list>
#include <map>
#include <string>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include "Condition.h"
#include <pthread.h>
#include <event2/util.h>
#include <event2/buffer.h>
#include <event2/keyvalq_struct.h>
#include <iostream>

using namespace std;    
class HttpContext{
public:
    HttpContext(struct evhttp_request * request);
    void Init();
    string GetParamValue(const string& param_name);
    string& GetUrl();
    string GetHeaderValue(const string& header_name);
    string& GetBody();
    map<string, string>& GetOutputHeaders();
    struct evhttp_request* GetEvhttpReq();
private:
    void ParseUri(const char* orig_uri);
private: 
    map<string, string> params_map_;
    map<string, string> headers_map_;
    string url_;//without params 
    struct evhttp_request * request_;
    string body_;
private:
    map<string, string> output_headers_map_;

};

struct PendingReply{
    ~PendingReply()
    {
        evbuffer_free(buffer_);
    }

    struct evhttp_request *req_;
    struct evbuffer *buffer_;
};
typedef std::tr1::shared_ptr<HttpContext> HttpContextPtr; 

class HttpService{
public:
    typedef std::tr1::function<void(string& response) > ResponseCallBack;

    typedef  std::tr1::function<void(HttpContextPtr& req, ResponseCallBack callback)> CallBackFun;
    //typedef  std::tr1::shared_ptr<PendingReply> PendingReplyPtr;
public:
    HttpService(struct event_base *base);
    int Listen(uint16_t port);
    void RegisterRequestCallback(string url, CallBackFun callback);
    void RegisterParseParams(string param);
    void SetParseParams();
    bool WhetherParseParams();
    //call the sendreply to send the response
    void SendReply(HttpContextPtr& ctx_ptr, const string& response);

private:
    typedef std::tr1::shared_ptr<PendingReply> PendingReplyPtr; 
    typedef std::tr1::shared_ptr<TimerEvent> TimerEventPtr;

    static void GenericCallBack(struct evhttp_request * req, void *arg);
    void SendPendingReply();
    static  void DefalutHttpResponse(struct evhttp_request * req,void *  arg);


private:
    struct event_base* base_;
    struct evhttp * http_;
    uint16_t port_;
    TimerEventPtr pending_reply_timer_;
    map<string, CallBackFun> callback_map_;
    list<PendingReplyPtr> pending_reply_list_;
    pthread_mutex_t reply_list_mutex_;   
    Condition condition_;
};

#endif
