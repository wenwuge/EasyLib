#include "HttpService.h"
#include <assert.h>

void echoresponse(HttpContextPtr& req, HttpService::ResponseCallBack callback)
{
    cout << req->GetUrl() << endl;
    string response = "echo success!";
    response.append(req->GetBody());
    callback(response);
}

int main(int argc , char** argv)
{
    struct event_base * base = event_base_new();
    
    assert(base!=NULL);

    HttpService service(base);

    service.Listen(8888);
    service.RegisterRequestCallback("/echo", std::tr1::bind(echoresponse, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

    event_base_dispatch(base);
    return 0;
}
