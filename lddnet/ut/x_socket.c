#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>




#include "x_socket.h"

void x_set_sock_default_opt(int s)
{
	//设置SO_REUSEADDR选项(服务器快速重起)
	int on = 1;
	int result = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (result < 0) {
		close(s);
		return ;
	}
	
	on = 1;
	result = setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof(on)); //keep alive in TCP connection
	
	//设置SO_LINGER选项(防范CLOSE_WAIT挂住所有套接字, 强行关闭)
	struct linger optval;
	optval.l_onoff = 1;
	optval.l_linger = 60;
	result = setsockopt(s, SOL_SOCKET, SO_LINGER, (char *)&optval, sizeof(struct linger));
	if ( result < 0 ) {
		close( s );
		return ;
	}
	
	int len = 2 * 1024 * 1024;
	setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&len, sizeof(len));
	setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&len, sizeof(len));
	
	return;
}


/* disable nagle on socket */
void x_set_sock_no_delay(int s)
{
	int opt;
	socklen_t optlen;

	optlen = sizeof opt;
	if (getsockopt(s, IPPROTO_TCP, 1/*TCP_NODELAY*/, &opt, &optlen) == -1) {
		return;
	}
	
	if (opt == 1) {
		return;
	}
	opt = 1;
	setsockopt(s, IPPROTO_TCP, 1/*TCP_NODELAY*/, &opt, sizeof( opt ));

	return;
}


int x_create_tcp_server(const char *lip, int lport)
{
	struct sockaddr_in addr;
	int s = socket( AF_INET, SOCK_STREAM, 0 );
	if ( s == -1 ) {
		return -1;
	}
	
	x_set_sock_default_opt(s);

	addr.sin_family = AF_INET;
	if (strcmp(lip, "0.0.0.0") == 0) {
		addr.sin_addr.s_addr = INADDR_ANY; //htonl(INADDR_ANY);
	}
	else {
		addr.sin_addr.s_addr = inet_addr( lip );
	}
	addr.sin_port = htons( lport );
	
	if (bind(s, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1 ) {
		close( s);
		s = -1;
		return -1;
	}
	
	if (listen(s, MAX_LISTEN_QUEUE ) == -1 ) {
		close(s);
		s = -1;
		return -1;
	}
	
	return s;
}


int x_create_tcp_client(const char *rip, int rport)
{
	struct sockaddr_in addr;
	int result;
	int on = 1;
	int fd;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if ( fd == -1 ) {
		return -1;
	}

	result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
	if (result < 0 ) {
		close(fd);
		fd = -1;
		return -1;
	}

	on = 1;
	result = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on)); //not use Nagle delay algorithm
	result = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*) & on, sizeof(on)); //keep alive in TCP connection

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(rip);
	addr.sin_port = htons(rport);
	result = connect(fd, (struct sockaddr *) & addr, sizeof(struct sockaddr));
	if ( result != 0 ) {
		close(fd);
		fd = -1;
		return -1;
	}

	return fd;
}

int x_accept(int listen_fd, char *ip, int *port)
{
	int fd = -1;
	struct sockaddr_in remote;
	int len = 0;

ReAccept:
	
	len = sizeof( struct sockaddr_in );
	
	fd = accept(listen_fd, (struct sockaddr *)&remote, (socklen_t *)&len);
	if (fd == -1) {
		int err = errno;
		if ( err == EINTR || err == EAGAIN ) {
			goto ReAccept;
		}
		
#ifdef  EPROTO
		if (err == EPROTO || err == ECONNABORTED) {
			goto ReAccept;
		}
#else
		if (err == ECONNABORTED ) {
			goto ReAccept;
		}
#endif
		return -1;
	}
	
	x_set_sock_no_delay(fd);

	if ( port != NULL ) {
		*port = ntohs(remote.sin_port);
	}
	
	if ( ip != NULL ) {
		strcpy( ip, inet_ntoa(remote.sin_addr));
	}
	
	return fd;
}


int x_close(int fd)
{
	shutdown(fd, SHUT_RDWR);
	
	close(fd);
	
	fd = -1;

	return 0;
}

int x_send(int fd, char *buf, int len)
{
	// if fd is closed, return bytes is -1
	int bytes = -1;

	int length = len;
	do {

ReSend:

		bytes = send(fd, buf, length, 0);
		if ( bytes > 0 ) {
			length -= bytes;
			buf += bytes;
		}

		else if (bytes == -1) {
			if (errno == EINTR) {
				goto ReSend;
			}
		}
		else {
			if (errno == EINTR || errno == EAGAIN) {
				goto ReSend;
			}
		}

	} while ( length > 0 && bytes > 0 );
	
	if ( length == 0 ) {
		bytes = len; // 已发送的字节数
	}

	return bytes;
}

int x_recvbytes(int fd, char *buf, int len)
{
	int bytes = -1;

	bytes = recv(fd, buf, len, 0);

	return bytes;
}

int x_recv(int fd, char *buf, int len)
{
	int bytes = -1; 

	int length = len;
	do {

		bytes = recv(fd, buf, length, 0);
		if ( bytes > 0 ) {
			length -= bytes;
			buf += bytes;
		}

	} while ( length > 0 && bytes > 0 );

	if ( length == 0 ) {
		bytes = len; // 已接收的字节数
	}

	return bytes;
}

/*
*notice : fd can not greate 1024 when use select as timer.
*/
int x_recv_with_timeout(int fd, char *buf, int len, int timeout)
{
	int bytes = -1;
	int length = len;
	
	if (timeout <= 0 ) {
		return x_recv(fd, buf, len);
	}

	if ( fd < 0 ) {
		return -1;
	}

	fd_set rset;
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	
	while ( tv.tv_sec > 0 && length > 0 && fd >= 0) {

		FD_ZERO(&rset);
		FD_SET(fd, &rset);
		
		int retval = select(fd + 1, &rset, NULL, NULL, &tv);
		if ( retval == 0 ) {
			//SAY( DP_DEBUG6, "<SYSTEM_WARN> Socket timeout when recving data\n" );
			break;
		}
		else if ( retval < 0 ) {
			if ( errno == EINTR ) {
				continue; // capture a signal
			}
			
			return -1; // socket error
		}

		bytes = recv(fd, buf, length, 0);
		if (bytes > 0) {
			length -= bytes;
			buf += bytes;
		}
		else if ( bytes == 0 ) { // the peer has performed an orderly shutdown.
			break;
		}
		else if (bytes == -1) {
			if ( errno == EINTR || errno == EAGAIN ) { // error occur, bytes<0
				continue;
			}

			return -1;// socket error
		}
	}
		

	bytes = len - length;
	
	return bytes;
}

