#include "HttpService.h"
HttpContext::HttpContext(struct evhttp_request * request)
{
    request_ = request;

}

void HttpContext::ParseUri(const char* orig_uri)
{
    string uri =orig_uri;
    int pos = 0;

    if((pos = uri.find("?")) != string::npos){
        url_ = uri.substr(0, pos);
    }else{
        url_ = uri;
    }
    
    int pos1 = pos + 1;
    while((pos =  uri.find("&", pos1)) != string::npos){
        int pos2 = uri.find("=", pos1);
        if(pos2 == string::npos){
            cout << "wrong uri:" << orig_uri <<endl;
            return;
        }
        string key = uri.substr(pos1, pos2 - pos1) ;
        string val = uri.substr(pos2 + 1, pos - pos2 -1) ;

        cout << "key: " << key << " val:" <<val << endl;
        params_map_[key] = val;
        pos1 = pos + 1;
    }

    if(pos1 != uri.size()){
        int pos2 = uri.find("=", pos1);
        if(pos2 == string::npos){
            cout << "wrong uri:" << orig_uri <<endl;
            return;
        }
        string key = uri.substr(pos1, pos2 - pos1) ;
        string val = uri.substr(pos2 + 1, uri.size() - pos2 -1) ;
        pos1 = pos + 1;
        cout << "key: " << key << " val:" <<val << endl;
        params_map_[key] = val;
    }

    return ;
}

void HttpContext::Init()
{
    const char* url = evhttp_request_get_uri(request_);
    struct evkeyvalq *headers = NULL;
    struct evkeyval *header = NULL;

    headers = evhttp_request_get_input_headers(request_);
    for(header = headers->tqh_first; header; header= header->next.tqe_next){
        headers_map_[header->key] = header->value;
    }
    
    //get url without params and params
    ParseUri(url);

    //get url body
    struct evbuffer* buf = NULL;
    buf = evhttp_request_get_input_buffer(request_);
    while(evbuffer_get_length(buf)){
        int n = 0;
        char cbuf[128];
        n = evbuffer_remove(buf, cbuf, sizeof(cbuf) -1);
        if(n > 0){
            body_.append(cbuf, 0, n);
        }
    }
    

}

string HttpContext::GetParamValue(const string &param_name)
{
    string ret;
    map<string ,string>::iterator iter;
    iter = params_map_.find(param_name);
    if(iter != params_map_.end()){
        return params_map_[param_name];
    }else{
        return "";
    }
}

string  HttpContext::GetHeaderValue(const string &header_name)
{
    string ret;
    map<string ,string>::iterator iter;
    iter = headers_map_.find(header_name);
    if(iter != headers_map_.end()){
        return headers_map_[header_name];
    }else{
        return "";
    }
}

string& HttpContext::GetUrl()
{
    return url_;
}

string& HttpContext::GetBody()
{
    return body_;
}

map<string, string>& HttpContext::GetOutputHeaders()
{
    return output_headers_map_;
}

struct evhttp_request* HttpContext::GetEvhttpReq()
{
    return request_;
}
HttpService::HttpService(struct event_base* base):port_(80),condition_(base)
{
    base_ = base;
    http_ = evhttp_new(base_);
    pthread_mutex_init(&reply_list_mutex_, NULL);
    condition_.Init();
}

int HttpService::Listen(uint16_t port)
{
    if(!base_ || !http_){
        cout << "base_ or http_ is empty" << endl;
        return -1;
    }

    evhttp_set_gencb(http_, GenericCallBack, this);
    if( evhttp_bind_socket(http_, "0.0.0.0", port) < 0){
        cout << "evhttp_bind_socket error" << endl;
        perror("evhttp_bind_socket error");
    }
    

    //pending reply set
    condition_.Watch(std::tr1::bind(&HttpService::SendPendingReply, this) ,0); 
    return 0;

}

void HttpService::GenericCallBack(struct evhttp_request * req, void * arg)
{
    string url =  req->uri;
    HttpContextPtr CtxPtr(new HttpContext(req));
    CtxPtr->Init();
    HttpService * service = (HttpService*)arg;

    if(service->callback_map_.find(CtxPtr->GetUrl()) != service->callback_map_.end()){
        service->callback_map_[CtxPtr->GetUrl()](CtxPtr, std::tr1::bind(&HttpService::SendReply,(HttpService*)arg, CtxPtr, std::tr1::placeholders::_1));
        return;
    }

    //send default response;
    evhttp_send_reply(req, HTTP_BADREQUEST, "BAD REQUEST", NULL); 


}
void HttpService::SendPendingReply()
{
   list<PendingReplyPtr> tmp;
   pthread_mutex_lock(&reply_list_mutex_);
   swap(tmp, pending_reply_list_);
   pthread_mutex_unlock(&reply_list_mutex_);

   list<PendingReplyPtr>::iterator begin(tmp.begin()),end(tmp.end());
    for(;begin != end; begin++){
       evhttp_send_reply((*begin)->req_, 200, "OK",(*begin)->buffer_); 
    }
}

//not real reply, only put the reply response into reply list
void HttpService::SendReply(HttpContextPtr& ctx_ptr,const string & response)
{
    PendingReplyPtr reply_ptr(new PendingReply);
    
    reply_ptr->req_ = ctx_ptr->GetEvhttpReq(); 
    reply_ptr->buffer_ = evbuffer_new();  
    
    //add content and headers;
    evbuffer_add(reply_ptr->buffer_, response.c_str(), response.size());
    map<string,string>::iterator begin = ctx_ptr->GetOutputHeaders().begin();
    map<string,string>::iterator end = ctx_ptr->GetOutputHeaders().end();
    for(; begin != end; begin ++){
        evhttp_add_header(evhttp_request_get_output_headers(reply_ptr->req_),
                    begin->first.c_str(), begin->second.c_str());       
    }

    pthread_mutex_lock(&reply_list_mutex_);
    pending_reply_list_.push_back(reply_ptr); 
    pthread_mutex_unlock(&reply_list_mutex_);
    condition_.Notify();
}

void HttpService::RegisterRequestCallback(string url, CallBackFun callback)
{
    callback_map_[url] = callback;
}
