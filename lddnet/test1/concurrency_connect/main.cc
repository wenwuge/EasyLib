#include <unistd.h>
#include <glog/logging.h>
#include <iostream>
#include "x_socket.h"

using namespace std;

int main(int argc, char *argv[])
{
    for (int i = 0; i < 100; i++) {
        int fd = x_create_tcp_client("127.0.0.1", 9527);
        if (fd == -1) {
            return -1;
        }

        usleep(100);

        /*3.关闭连接*/
        x_close(fd);
    }

    return 0;
}

