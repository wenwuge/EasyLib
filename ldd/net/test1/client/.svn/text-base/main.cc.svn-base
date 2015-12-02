#include <unistd.h>
#include <glog/logging.h>
#include <iostream>
#include "common.h"
#include "x_socket.h"

using namespace std;

void Usage(char *instancename)
{
    cout << "Usage : " << instancename << " <host> <port> <test item>" << endl;
    cout << endl;
    cout << "test item list:" << endl;
    cout << "1.HeartHeat" << endl;
    cout << "2.Get" << endl;
    cout << "3.Put" << endl;
    cout << endl;
    return;
}

typedef void (*TestFunc)(int fd);

struct TestItem {
    char *name;
    TestFunc func;
};

TestItem g_test_items[] = {
    {"HeartBeat", HeartBeat},
    {"Get", Get},
    {"Put", Put}
};

void Run(int fd, string &test_item)
{
    for (int i = 0; i < (int)(sizeof(g_test_items)/sizeof(TestItem)); i++) {
        if (strcmp(test_item.c_str(), g_test_items[i].name) == 0) {
            g_test_items[i].func(fd);
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        Usage(argv[0]);
        return 0;
    }

    string host(argv[1]);
    int port = atoi(argv[2]);
    string test_item(argv[3]);

    cout << "host     : " << host << endl;
    cout << "port     : " << port << endl;
    cout << "test item: " << test_item << endl;

    /*1.创建连接*/
    int fd = x_create_tcp_client(host.c_str(), port);
    if (fd == -1) {
        cout << "connet to " << host << ":" << port << " failed" << endl;
        return -1;
    }

    /*2.运行测试用例*/
    Run(fd, test_item);

    /*3.关闭连接*/
    x_close(fd);

    return 0;
}

