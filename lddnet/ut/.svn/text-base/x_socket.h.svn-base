#ifndef _X_SOCKET_H__
#define _X_SOCKET_H__

#define TTL_VALUE 255

#define SOCK_TYPE_NULL					0
#define SOCK_TYPE_UNICAST_TCP_SERVER	1
#define SOCK_TYPE_UNICAST_TCP_CLIENT	2
#define SOCK_TYPE_UNICAST_UDP_SERVER	3
#define SOCK_TYPE_UNICAST_UDP_CLIENT	4
#define SOCK_TYPE_FILEOUTPUT			5
#define SOCK_TYPE_THIS_LINK				6

#define MAX_LISTEN_QUEUE				5

#ifdef __cplusplus
extern "C" {
#endif

void x_set_sock_default_opt(int s);
void x_set_sock_no_delay(int s);

int x_create_tcp_server(const char *lip, int lport);
int x_accept(int listen_fd, char *ip, int *port);
int x_create_tcp_client(const char *rip, int rport);
int x_close(int fd);
int x_send(int fd, char *buf, int len);
int x_recv(int fd, char *buf, int len);
int x_recvbytes(int fd, char *buf, int len);
int x_recv_with_timeout(int fd, char *buf, int len, int timeout);


#ifdef __cplusplus
}
#endif

#endif /*_X_SOCKET_H__*/

