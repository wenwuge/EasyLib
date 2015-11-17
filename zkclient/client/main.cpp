#include "SyncZkClient.h"
#include <unistd.h>
#include <iostream>


void InitWatch(zhandle_t* zh, int type, int state, const char* path, void* watcher) 
{
    if (type == ZOO_SESSION_EVENT) {
        if (state == ZOO_CONNECTED_STATE) {
            cout << "build connection ok" << endl;
        } else if (state == ZOO_EXPIRED_SESSION_STATE) {
            cout << "connection disconnect" << endl;
            //zkhandle = zookeeper_init(host, InitWatch, timeout, 0, const_cast<char*>("TODO"), 0);
            ZkClient * tmp =(ZkClient*)watcher;
            tmp->Init();
        }
    }
}

void ChildWatch(zhandle_t* zh, int type, int state, const char* path, void* watcher) {
    if (type == ZOO_CHILD_EVENT) {
#if 0
        struct String_vector sddtr_vec;
        
        memset(&str_vec, 0 ,sizeof(struct String_vector));
        int ret = zoo_wget_children(zh, "/", ChildWatch, 0, &str_vec);
        if (ret) {
            cout << "zoo_wget_children error :" << ret << endl;
        }
#else

        string parent_path = "/";
        vector<string> results;
        ZkClient * tmp = (ZkClient*)watcher;
        int ret = tmp->GetChildrenNodes(parent_path, ChildWatch, results);

#endif

        cout << "endpoint change: " << endl;
        for (int i = 0; i < results.size(); ++i) {
            string endpoint = results[i]; 
            cout << endpoint << endl;
        }
    }else{
        
        cout << "endpoint change: " << endl;
    }
}

int main(int argc, char** argv)
{
    ZkClient sclient("127.0.0.1:4180,127.0.0.1:4181,127.0.0.1:4182", InitWatch, 100);
 
    sclient.Init();
     
    int ret = 0;
    string parent_path = "/tmp";
    vector<string> results;
    ret = sclient.GetChildrenNodes(parent_path, ChildWatch, results);
    for (int i = 0; i < results.size(); ++i) {
        string endpoint = results[i]; 
        cout << "init endpoint :" << endpoint << endl;
        
        string value;
        struct Stat stat;
        endpoint = parent_path + endpoint;
        sclient.GetNodeInfoW(endpoint, ChildWatch, value, stat);
        cout << "value: " << value << endl;
    //    sleep(120);
    }


    while(1)
        sleep(10);
}
