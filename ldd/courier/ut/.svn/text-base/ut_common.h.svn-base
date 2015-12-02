

#ifndef _LDD_UT_COMMON_H_
#define _LDD_UT_COMMON_H_

#include <stdio.h>
#include <errno.h>
#include <string>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>



/************************************************************************/
/* define int broker                                                    */
/************************************************************************/
extern const char* BASE_CONF_PATH		;
extern const char* REPLICA_NODE_PATH	;
extern const char* ENGINE_NODE_PATH		;
extern const char* BIN_LOG_PATH			;
extern const char* BUCKET_NODE_PATH		;
extern const char* NODE_LIST_PATH		;
extern const char* BUCKET_LIST_PATH		;
extern const char* FARM_LIST_PATH		;
extern const char* COORDINATION			;
extern const char* ROLES_PATH			;
extern const char* VERSION_PATH			;
extern const char* ALIVE_NODES_LIST_PATH		;

extern const char* NAME_SPACE_LIST_PATH;
extern const char*	NAME_SPACE_1			   ;
extern const char* NAME_SPACE_2			   ;
/************************************************************************/
/* end comment                                                          */
/************************************************************************/

extern const char* REPLICA_NODE_PATH__VALUE	;
extern const char* ENGINE_NODE_PATH__VALUE	;
extern const char* BIN_LOG_PATH__VALUE		;
extern const char* BUCKET_NODE_PATH__VALUE  ; 


extern const int   NODE_LIST_SIZE ;
extern const char* HOST_PORT0 ;
extern const char* HOST_PORT1 ;
extern const char* HOST_PORT2 ;
extern const char* HOST_PORT0_VALUE ;
extern const char* HOST_PORT1_VALUE ;
extern const char* HOST_PORT2_VALUE ;

extern const char* FARM_0_VALUE ;
extern const char* FARM_1_VALUE ;

extern const char* BUCKET_0 ;
extern const char* BUCKET_1;
extern const char* BUCKET_2;
extern const char* BUCKET_0_VALUE;
extern const char* BUCKET_1_VALUE;
extern const char* BUCKET_2_VALUE;



enum ZkSyncOperationType{
	kcreateOptCom				=1,
	kdeleteOptCom				=2,	
	kgetOptCom				    =3,
	kgetChildrenOptCom		    =4,
	kexistsOptCom			    =5,
	ksetOptCom			        =6
};

#pragma pack(1)
struct ZkOptBody_{
	ZkOptBody_(): opt_type(0), path_len(0), value_len(0), flags(0){
	}
	int8_t  opt_type;
	int32_t path_len;
	char path[500];
	int32_t value_len;
	char value[2000];
	int32_t flags;
};
typedef struct ZkOptBody_ ZkOptBody;
#pragma pack()

extern int						ut_socket_port ;
extern int						client_socket  ;
extern struct sockaddr_in zkStationAddr;



extern int		   station_socket_port ;

void InitializeZkTree(int client_socket, struct sockaddr_in& zkStationAddr);

void ClearZkTree(int client_socket, struct sockaddr_in& zkStationAddr);

int  CreateUdpSocket(int port=0);

int SendUDP(int sock, struct sockaddr_in* toAddress,  const char*buf, int buf_len);

int SetAddress(struct sockaddr_in& stSrvAddr, char* ip, int port=0);

int sendAndRecv(int sock, struct sockaddr_in* toAddress,  const char*buf, int buf_len,
				char* recvbf, int recv_len);

#endif  // _LDD_UT_COMMON_H_


