#include "SyncZkClient.h"
#include <unistd.h>
#include <iostream>

ZkClient * tmp = NULL;

void InitWatch(zhandle_t* zh, int type, int state, const char* path, void* watcher) 
{
    if (type == ZOO_SESSION_EVENT) {
        if (state == ZOO_CONNECTED_STATE) {
            cout << "build connection ok" << endl;
        } else if (state == ZOO_EXPIRED_SESSION_STATE) {
            cout << "connection disconnect" << endl;
            //zkhandle = zookeeper_init(host, InitWatch, timeout, 0, const_cast<char*>("TODO"), 0);
            tmp->Init();
        }
    }
}

int main(int argc, char** argv)
{
    ZkClient sclient("127.0.0.1:4180,127.0.0.1:4181,127.0.0.1:4182", InitWatch, 100);
 
    sclient.Init();
    tmp = &sclient;
    string val1 = "1.1.1.1";
    string val2 = "2.2.2.2";
    string path = "/tmp";
    string path1 = path + "1";

    sclient.RegisterNode(path, val1, 0); 
    sclient.RegisterNode(path1, val2, ZOO_EPHEMERAL);

    while(1)
        sleep(10);
}
