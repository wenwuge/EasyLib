#include "HttpService.h"
#include <assert.h>
#include "EasyHttp.h"
void echoresponse(HttpContextPtr& req, HttpService::ResponseCallBack callback)
{
    //cout << req->GetUrl() << endl;
    string response = "echo success!";
    response.append(req->GetBody());
    callback(response);
}

int main(int argc , char** argv)
{
#if 0
    struct event_base * base = event_base_new();
    
    assert(base!=NULL);

    HttpService service(base);

    service.Listen(8888);
    service.RegisterRequestCallback("/echo", std::tr1::bind(echoresponse, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

    event_base_dispatch(base);
    return 0;
#else
    HttpServer server(8);
    server.Init();
    server.Listen(8888);
    string url = "/echo";
    server.RegisterUrlProcessor(url, std::tr1::bind(echoresponse, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    server.Start();
    int i = 3;
    while(i--){
        cout << "sleep 10s" << endl;
        sleep(1);
    }
    server.Stop();
    cout << "have stop the server" << endl;
        sleep(1);
    cout << "after sleep" <<endl;
    return 0;
#endif
}
