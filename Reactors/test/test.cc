#include "../net/tcpserver.h"
#include <iostream>

int main(int argc, char** argv)
{
    TcpServer server;

    struct Options options;
    options.thread_num_ = 2;
    options.port_ = 8899;
    options.addr_ = "127.0.0.1";
    
    server.SetOptions(options);
    server.Start();
    return 0;
    
}
