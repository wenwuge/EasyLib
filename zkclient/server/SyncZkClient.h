#ifndef __SYNC_ZKCLIENT
#define __SYNC_ZKCLIENT

#include <zookeeper/zookeeper.h>
#include <string>
#include <vector>

using namespace std;

class ZkClient {
    #define BUF_LEN (1024*4)
    public:
        typedef void(*WatchCallback)(zhandle_t* , int , int , const char*, void*);
        ZkClient(string host , watcher_fn func, int timeout, int flags=0);
        ~ZkClient();

        bool Init();
    public:
        //usally used by server point
        bool RegisterNode(string& path, string& value, int expire_flag) ;
        int DeleteNode();
    public:
        //usally used by client point
        int GetChildrenNodes(string& parent_path, WatchCallback watch_callback, vector<string>& children_nodes); 
        int GetNodeInfo(string& node_path,string& buffer,  struct Stat &stat);
        int GetNodeInfoW(string& node_path, WatchCallback watch_callback, string& buffer, struct Stat &stat);
    private:
        string host_;
        zhandle_t *handle_;
        watcher_fn watch_callback_;
        int     timeout_;
        int     flags_;
};

#endif
