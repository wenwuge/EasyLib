#include "SyncZkClient.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;

ZkClient::ZkClient(string host , watcher_fn func, int timeout, int flags):host_(host), handle_(NULL),
    watch_callback_(func),timeout_(timeout), flags_(flags)
{
        
}

ZkClient::~ZkClient()
{
    if (handle_)
        zookeeper_close(handle_);
    handle_ = NULL;
}

bool ZkClient::Init()
{
    
    printf("@@@@####host is : %s\n", host_.c_str());
    handle_ = zookeeper_init(host_.c_str(), watch_callback_ , timeout_, 0, (char*)"TODO", flags_);

    if (!handle_){
        cout << "zookeeper init error!" << endl; 
        return false; 
    }

    return true;
}

bool ZkClient::RegisterNode(string& path, string& value, int expire_flag)
{
    char newpath[128] = {0};

    int rc = zoo_create(handle_, path.c_str(), value.c_str(), value.size(), &ZOO_OPEN_ACL_UNSAFE, expire_flag, newpath , 128);
    
    cout << " rc is "<< rc <<endl;
    cout << " newpath is :" << newpath <<endl;
    if (rc){
        cout << "error : RegisterNode "<< path << endl;
        return false;
    }

    return true;
}

int ZkClient::GetChildrenNodes(string& parent_path, WatchCallback watch_callback, vector<string>& children_nodes)
{
    int ret = 0;
    struct String_vector str_vec;
    memset(&str_vec, 0 ,sizeof(struct String_vector));

    ret = zoo_wget_children(handle_, parent_path.c_str(), watch_callback, this, &str_vec);
    
    for(int i = 0; i < str_vec.count; i++){
        children_nodes.push_back(str_vec.data[i]); 
    }
    return ret;
}

int ZkClient::GetNodeInfo(string& node_path,string& buffer,  struct Stat &stat)
{
    char * buf = (char*)malloc(BUF_LEN);
    if(!buf){
        cout << "malloc buf error!" << endl;
        return -1;
    }
    int len = BUF_LEN;
    int ret = zoo_get(handle_, node_path.c_str(), 0, buf, &len, &stat);
    cout << "node_path is : " << node_path << " buf_len: " << len << endl;
    buf[len] = '\0';
    buffer = buf;
 
    free(buf); 
    return ret; 
}

int ZkClient::GetNodeInfoW(string& node_path,WatchCallback watch_callback, string& buffer, struct Stat &stat)
{
    char * buf = (char*)malloc(BUF_LEN);
    if(!buf){
        cout << "malloc buf error!" << endl;
        return -1;
    }
    int len = BUF_LEN;
    int ret = zoo_wget(handle_, node_path.c_str(), watch_callback  ,this, buf, &len, &stat);
    buf[len] = '\0';
    buffer = buf;
    free(buf); 
    return ret; 
}
