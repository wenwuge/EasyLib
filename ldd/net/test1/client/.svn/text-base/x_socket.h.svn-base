#ifndef _X_SOCKET_H__
#define _X_SOCKET_H__

#define TTL_VALUE 255

#define MAX_LISTEN_QUEUE				5

#ifdef __cplusplus
extern "C" {
#endif

void x_set_sock_default_opt(int s);
void x_set_sock_no_delay(int s);
int x_set_sock_no_block(int s);

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

