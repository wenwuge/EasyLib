#include <string.h>
#include <stdlib.h>

#include "ut_common.h"




int						station_socket_port = 30000;

const char* BASE_CONF_PATH			= "/confs";
const char* REPLICA_NODE_PATH		= "/replicas";
const char* ENGINE_NODE_PATH		= "/engine";
const char* BIN_LOG_PATH			= "/binlog_max";
const char* BUCKET_NODE_PATH		= "/buckets";
const char* NODE_LIST_PATH			= "/nodes";
const char* BUCKET_LIST_PATH		= "/buckets";
const char* FARM_LIST_PATH			= "/farms";
const char* COORDINATION			= "/coordinator";
const char* ROLES_PATH				= "/roles";
const char* VERSION_PATH			= "/version";
const char* ALIVE_NODES_LIST_PATH	= "/alive_nodes";


const char* NAME_SPACE_LIST_PATH			= "/ns";
const char*	NAME_SPACE_1					= "ns1";
const char* NAME_SPACE_2					= "ns2";


const char* REPLICA_NODE_PATH__VALUE	= "2";
const char* ENGINE_NODE_PATH__VALUE		= "leveldb";
const char* BIN_LOG_PATH__VALUE			= "65536";
const char* BUCKET_NODE_PATH__VALUE		= "1024";



const char* FARM_0_VALUE = "0";
const char* FARM_1_VALUE = "1";

const int NODE_LIST_SIZE = 3;

const char* HOST_PORT0 = "10.16.28.11:40000";
const char* HOST_PORT1 = "10.16.28.11:40001";
const char* HOST_PORT2 = "10.16.28.11:40002";
const char* HOST_PORT0_VALUE = FARM_0_VALUE;
const char* HOST_PORT1_VALUE = FARM_1_VALUE;
const char* HOST_PORT2_VALUE = FARM_0_VALUE;

//const std::string host_port = "10.16.28.11:2181";

const char* BUCKET_0  = "/0";
const char* BUCKET_1  = "/1";
const char* BUCKET_2  = "/2";
const char* BUCKET_0_VALUE = FARM_0_VALUE;
const char* BUCKET_1_VALUE = FARM_1_VALUE;
const char* BUCKET_2_VALUE = FARM_0_VALUE;




void InitializeZkTree(int client_socket, struct sockaddr_in& zkStationAddr)
{	static char send_buffer[65535]; 
	static char recv_buffer[65535];
	int recv_buffer_len = 65535;
	int recv_len = 0;
	strcpy(send_buffer, "ClearZkTree");
	recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, strlen("ClearZkTree"),recv_buffer, recv_buffer_len);
	if (recv_len <=0 || strncmp(recv_buffer, "OK!!!", strlen("OK!!!")) !=0 )
	{ fprintf(stderr, "ERROR sendAndRecv ERROR in station, file=%s, line=%d\n", __FILE__, __LINE__); exit(0);}

	strcpy(send_buffer, "CreateConfsOnZkTree");
	recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, strlen("CreateConfsOnZkTree"),recv_buffer, recv_buffer_len);
	if (recv_len <=0 || strncmp(recv_buffer, "OK!!!", strlen("OK!!!")) !=0 ){ fprintf(stderr, "ERROR sendAndRecv ERROR in station, file=%s, line=%d\n", __FILE__, __LINE__); exit(0);}


	strcpy(send_buffer, "CreateNodesAndChildren");
	recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, strlen("CreateNodesAndChildren"),recv_buffer, recv_buffer_len);
	if (recv_len <=0 || strncmp(recv_buffer, "OK!!!", strlen("OK!!!")) !=0 ){ fprintf(stderr, "ERROR sendAndRecv ERROR in station, file=%s, line=%d\n", __FILE__, __LINE__); exit(0);}

	strcpy(send_buffer, "CreatebucketsAndChildren");
	recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, strlen("CreatebucketsAndChildren"),recv_buffer, recv_buffer_len);
	if (recv_len <=0 || strncmp(recv_buffer, "OK!!!", strlen("OK!!!")) !=0 ){ fprintf(stderr, "ERROR sendAndRecv ERROR in station, file=%s, line=%d\n", __FILE__, __LINE__); exit(0);}

	strcpy(send_buffer, "CreatefarmsAndChildren");
	recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, strlen("CreatefarmsAndChildren"),recv_buffer, recv_buffer_len);
	if (recv_len <=0 || strncmp(recv_buffer, "OK!!!", strlen("OK!!!")) !=0 ){ fprintf(stderr, "ERROR sendAndRecv ERROR in station, file=%s, line=%d\n", __FILE__, __LINE__); exit(0);}
	usleep(100000);
}

void ClearZkTree(int client_socket, struct sockaddr_in& zkStationAddr)
{
	static char send_buffer[65535]; 
	static char recv_buffer[65535];
	int recv_buffer_len = 65535;
	int recv_len = 0;
	strcpy(send_buffer, "ClearZkTree");
	recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, strlen("ClearZkTree"),recv_buffer, recv_buffer_len);
	if (recv_len <=0 || strncmp(recv_buffer, "OK!!!", recv_len) !=0 )
	{ fprintf(stderr, "ERROR sendAndRecv ERROR in station\n"); exit(0);}
}


int  CreateUdpSocket(int port)
{
	int iListenSocket;
	iListenSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (iListenSocket < 0) {
		fprintf(stderr, "ERROR create socket");
		return -__LINE__;
	}

	if (port <=0)
	{
		return iListenSocket;
	}

	struct sockaddr_in ssin;
	memset(&ssin, 0, sizeof(struct sockaddr_in));
	ssin.sin_addr.s_addr = INADDR_ANY;
	ssin.sin_port = htons(port);

	int iRet = 0;
	iRet = bind(iListenSocket, (struct sockaddr *)&ssin, sizeof(struct sockaddr_in));
	if ( iRet < 0)
	{
		close(iListenSocket);
		fprintf(stderr, "bind ERROR iRet=%d", iRet);
		return -__LINE__;;
	}

	return iListenSocket;
}

int SendUDP(int sock, struct sockaddr_in* toAddress,  const char*buf, int buf_len)
{
	int nSentLength = 0;	
	nSentLength = sendto(sock, buf, buf_len, 0, (struct sockaddr*)toAddress, sizeof(struct sockaddr_in));
	if(nSentLength < 0)
	{
		//printf("ERROR to send msg, nSentLength=%d, ip(%s):port(%d)\n", nSentLength, inet_ntoa(toAddress->sin_addr),ntohs(toAddress->sin_port));
		return nSentLength;
	}
	//printf("nSendLength=%d, ip(%s):port(%d) \n", nSentLength, inet_ntoa(toAddress->sin_addr), ntohs(toAddress->sin_port));

	return 0;
}

int SetAddress(struct sockaddr_in& stSrvAddr, char* ip, int port)
{
	struct in_addr stTempAddr;   // another address
	memset(&stTempAddr, 0, sizeof(stTempAddr));
	stTempAddr.s_addr = inet_addr(ip);

	printf("input lIP = %s\n", ip);

	//port will be set in future

	memset(&stSrvAddr, 0, sizeof(stSrvAddr));
	stSrvAddr.sin_family	= AF_INET;
	stSrvAddr.sin_addr		= stTempAddr;
	stSrvAddr.sin_port		= htons(port);

	return 0;
}

int sendAndRecv(int sock, struct sockaddr_in* toAddress,  const char*buf, int buf_len,
				char* recvbf, int recv_len)
{
	int nSentLength = 0;	
	int nRecvLength = 0;
	int iRet=0;
	struct sockaddr_in fromAddress;
	socklen_t iAddressLen = sizeof(struct sockaddr_in);

	nSentLength = sendto(sock, buf, buf_len, 0, (struct sockaddr*)toAddress, sizeof(struct sockaddr_in));
	if(nSentLength < 0)
	{
		printf("ERROR to send msg, nSentLength=%d, ip(%s):port(%d)\n", nSentLength, inet_ntoa(toAddress->sin_addr),ntohs(toAddress->sin_port));
		return nSentLength;
	}

	//printf("nSendLength=%d, ip(%s):port(%d) \n", nSentLength, inet_ntoa(toAddress->sin_addr), ntohs(toAddress->sin_port));

	while(1)
	{
		fd_set fds;
		struct timeval tv;
		FD_ZERO(&fds);
		FD_SET(sock, &fds);
		tv.tv_sec=0;
		tv.tv_usec=5000000; // Ä¬ÈÏ1s
		iRet = select(sock+1,&fds,NULL,NULL,&tv);
		if(iRet<0 )
		{
			if( errno==EAGAIN || errno==EINTR )
				continue;
		}else if(iRet==0) // timeout Ô­Ñùcopy
		{
			printf("select timerout, timeout=%d, iRet=%d\n", (int)tv.tv_sec, iRet);
			return nRecvLength;
		}
		break;
	}

	nRecvLength = recvfrom(sock, recvbf, recv_len, 0, (struct sockaddr*)&fromAddress, &iAddressLen);
	if (nRecvLength < 0)
	{
		printf("recvfrom ERROR");
	}
	//printf("nRecvLength=%d, ip(%s):port(%d) \n", nRecvLength, inet_ntoa(fromAddress.sin_addr), ntohs(fromAddress.sin_port));
	return nRecvLength;
}



