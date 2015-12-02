
#include <zookeeper/zookeeper.h>		//using multithread zk
#include <gflags/gflags.h>
#include "ut_common.h"


static const int 		g_buffer_len = 10000;
static char				g_buffer[g_buffer_len];
zhandle_t* 				g_zhandle_;

static const int 		g_send_buffer_len = 10000;
static char				g_send_buffer[g_send_buffer_len];

DEFINE_string(host_port, "10.16.28.11:2182", "zookeeper host port");


void clearNodesTree(zhandle_t* handle){
	int ret = 0;  int len = 0;
	char path[100];
	
	len = snprintf(path, 100, "%s/%s", NODE_LIST_PATH, HOST_PORT0);
	if(len == 0 ){fprintf(stderr, "snprintf ERROR\n");}
	ret = zoo_delete(handle,path, -1);
	fprintf(stderr, "clearNodesTree() %s, ret=%d\n", path, ret);

	len = snprintf(path, 100, "%s/%s", NODE_LIST_PATH, HOST_PORT1);
	if(len == 0 ){fprintf(stderr, "snprintf ERROR\n");}
	ret = zoo_delete(handle,path, -1);
	fprintf(stderr, "clearNodesTree() %s, ret=%d\n", path, ret);

	len = snprintf(path, 100, "%s/%s", NODE_LIST_PATH, HOST_PORT2);
	if(len == 0 ){fprintf(stderr, "snprintf ERROR\n");}
	ret = zoo_delete(handle,path, -1);
	fprintf(stderr, "clearNodesTree() %s, ret=%d\n", path, ret);

	len = snprintf(path, 100, "%s", NODE_LIST_PATH);
	ret = zoo_delete(handle,path, -1);
	fprintf(stderr, "clearNodesTree() %s, ret=%d\n", path, ret); // delete parent
}

void clearBucketTree(zhandle_t* handle){
	int ret = 0;  int len = 0;
	char path[100];

	len = snprintf(path, 100, "%s%s", BUCKET_LIST_PATH, BUCKET_0);
	ret = zoo_delete(handle,path, -1);
	fprintf(stderr, "clearBucketTree() %s, ret=%d\n", path, ret);

	len = snprintf(path, 100, "%s%s", BUCKET_LIST_PATH, BUCKET_1);
	ret = zoo_delete(handle,path, -1);
	fprintf(stderr, "clearBucketTree() %s, ret=%d\n", path, ret);

	len = snprintf(path, 100, "%s%s", BUCKET_LIST_PATH, BUCKET_2);
	ret = zoo_delete(handle,path, -1);
	fprintf(stderr, "clearBucketTree() %s, ret=%d\n", path, ret);


	len = snprintf(path, 100, "%s", BUCKET_LIST_PATH);		/// delete parent
		ret = zoo_delete(handle,path, -1);
	fprintf(stderr, "clearBucketTree() %s, ret=%d\n", path, ret);

}

void clearFarmsFarm0Roles(zhandle_t* handle){

	char path[512];		struct String_vector role_st;
	snprintf(path, sizeof(path), "%s/%s%s", FARM_LIST_PATH, FARM_0_VALUE, ROLES_PATH);
	int ret =zoo_get_children(handle, path, 0, &role_st);
	if (role_st.count > 0 && ret==ZOK){
		for (int i=0 ; i< role_st.count; i++){
			snprintf(path, sizeof(path), "%s/%s%s/%s", FARM_LIST_PATH, FARM_0_VALUE, ROLES_PATH, role_st.data[i]);
			int ret = zoo_delete(handle,path, -1);
			fprintf(stderr, "clearFarmsFarm0Roles() %s, ret=%d\n", path, ret);
		}
	}
}

int ClearZkTree(zhandle_t* handle){
	int ret = 0;  int len = 0;
	char path[100];
	len = snprintf(path, 100, "%s%s", BASE_CONF_PATH, REPLICA_NODE_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");
	}
	ret = zoo_delete(handle, path, -1);

	len = snprintf(path, 100, "%s%s", BASE_CONF_PATH, ENGINE_NODE_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");
	}
	ret = zoo_delete(handle, path, -1);

	len = snprintf(path, 100, "%s%s", BASE_CONF_PATH, BIN_LOG_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");
	}
	ret = zoo_delete(handle, path, -1);

	len = snprintf(path, 100, "%s%s", BASE_CONF_PATH, BUCKET_NODE_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");
	}
	ret = zoo_delete(handle, path, -1);

	clearNodesTree(handle);
	clearBucketTree(handle);
	clearFarmsFarm0Roles(handle);
	return 0;
}

int CreateConfsOnZkTree(zhandle_t* handle){
	
	int ret = 0;  int len = 0;
	char path[100];
	char path_buffer[100]; int path_len = 100;

	len = snprintf(path, 100, "%s", BASE_CONF_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n"); 
	}
	ret = zoo_create(handle, path, "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreateConfsOnZkTree() %s, ret=%d\n", path, ret);


	len = snprintf(path, 100, "%s%s", BASE_CONF_PATH, REPLICA_NODE_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n"); 
	}
	ret = zoo_create(handle, path, REPLICA_NODE_PATH__VALUE, strlen(REPLICA_NODE_PATH__VALUE),
		&ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreateConfsOnZkTree() %s, ret=%d\n", path, ret);


	len = snprintf(path, 100, "%s%s", BASE_CONF_PATH, ENGINE_NODE_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");
	}
	ret = zoo_create(handle, path, ENGINE_NODE_PATH__VALUE, strlen(ENGINE_NODE_PATH__VALUE),
		&ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreateConfsOnZkTree() %s, ret=%d\n", path, ret);


	len = snprintf(path, 100, "%s%s", BASE_CONF_PATH, BIN_LOG_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");
	}
	ret = zoo_create(handle, path, BIN_LOG_PATH__VALUE, strlen(BIN_LOG_PATH__VALUE),
		&ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreateConfsOnZkTree() %s, ret=%d\n", path, ret);

	len = snprintf(path, 100, "%s%s", BASE_CONF_PATH, BUCKET_NODE_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");
	}
	ret = zoo_create(handle, path, BUCKET_NODE_PATH__VALUE, strlen(BUCKET_NODE_PATH__VALUE),
		&ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreateConfsOnZkTree() %s, ret=%d\n", path, ret);
 


	len = snprintf(path, 100, "%s", NAME_SPACE_LIST_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n"); 
	}
	ret = zoo_create(handle, path, "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreateConfsOnZkTree() %s, ret=%d\n", path, ret);

	len = snprintf(path, 100, "%s/%s", NAME_SPACE_LIST_PATH, NAME_SPACE_1);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n"); 
	}
	ret = zoo_create(handle, path, "1",2, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreateConfsOnZkTree() %s, ret=%d\n", path, ret);


	len = snprintf(path, 100, "%s/%s", NAME_SPACE_LIST_PATH, NAME_SPACE_2);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n"); 
	}
	ret = zoo_create(handle, path, "2", 2, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreateConfsOnZkTree() %s, ret=%d\n", path, ret);


	
	return 0;
}

void CreateNodesAndChildren(zhandle_t* handle){
	
	int ret = 0;  int len = 0;
	char path[100];
	char path_buffer[100]; int path_len = 100;
	len = snprintf(path, 100, "%s", NODE_LIST_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n"); 
	}
	ret = zoo_create(handle, path, "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreateNodesAndChildren() %s, ret=%d\n", path, ret);

	len = snprintf(path, 100, "%s/%s", NODE_LIST_PATH, HOST_PORT0);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");}
	ret = zoo_create(handle, path, HOST_PORT0_VALUE, strlen(HOST_PORT0_VALUE),
		&ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreateNodesAndChildren() %s, ret=%d\n", path, ret);

	len = snprintf(path, 100, "%s/%s", NODE_LIST_PATH, HOST_PORT1);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");}
	ret = zoo_create(handle, path, HOST_PORT1_VALUE, strlen(HOST_PORT1_VALUE),
		&ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreateNodesAndChildren() %s, ret=%d\n", path, ret);

	len = snprintf(path, 100, "%s/%s", NODE_LIST_PATH, HOST_PORT2);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");}
	ret = zoo_create(handle, path, HOST_PORT2_VALUE, strlen(HOST_PORT2_VALUE),
		&ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreateNodesAndChildren() %s, ret=%d\n", path, ret);
}

void CreatebucketsAndChildren(zhandle_t* handle){

	int ret = 0;  int len = 0;
	char path[100];
	char path_buffer[100]; int path_len = 100;
	len = snprintf(path, 100, "%s", BUCKET_LIST_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n"); 
	}
	ret = zoo_create(handle, path, "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreatebucketsAndChildren() %s, ret=%d\n", path, ret);


	len = snprintf(path, 100, "%s%s", BUCKET_LIST_PATH, BUCKET_0);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");}
	ret = zoo_create(handle, path, BUCKET_0_VALUE, strlen(BUCKET_0_VALUE),
		&ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreatebucketsAndChildren() %s, ret=%d\n", path, ret);


	len = snprintf(path, 100, "%s%s", BUCKET_LIST_PATH, BUCKET_1);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");}
	ret = zoo_create(handle, path, BUCKET_1_VALUE, strlen(BUCKET_1_VALUE),
		&ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreatebucketsAndChildren() %s, ret=%d\n", path, ret);


	len = snprintf(path, 100, "%s%s", BUCKET_LIST_PATH, BUCKET_2);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");}
	ret = zoo_create(handle, path, BUCKET_2_VALUE, strlen(BUCKET_2_VALUE),
						&ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreatebucketsAndChildren() %s, ret=%d\n", path, ret);
}

void CreatefarmsAndChildren(zhandle_t* handle){

	int ret = 0;  int len = 0;
	char path[100];
	char path_buffer[100]; int path_len = 100;
	len = snprintf(path, 100, "%s", FARM_LIST_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n"); 
	}
	ret = zoo_create(handle, path, "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreatebucketsAndChildren() %s, ret=%d\n", path, ret);


	len = snprintf(path, 100, "%s/%s", FARM_LIST_PATH, FARM_0_VALUE);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");}
	ret = zoo_create(handle, path, "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreatebucketsAndChildren() %s, ret=%d\n", path, ret);
	{
		len = snprintf(path, 100, "%s/%s%s", FARM_LIST_PATH, FARM_0_VALUE, ALIVE_NODES_LIST_PATH);
		if(len == 0 ){fprintf(stderr, "snprintf ERROR\n");}
		ret = zoo_create(handle, path, "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
		fprintf(stderr, "CreatebucketsAndChildren() %s, ret=%d\n", path, ret);

		len = snprintf(path, 100, "%s/%s%s", FARM_LIST_PATH, FARM_0_VALUE, COORDINATION);
		ret = zoo_create(handle, path, "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
		len = snprintf(path, 100, "%s/%s%s", FARM_LIST_PATH, FARM_0_VALUE, ROLES_PATH);
		ret = zoo_create(handle, path, "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	}

	len = snprintf(path, 100, "%s/%s", FARM_LIST_PATH, FARM_1_VALUE);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");}
	ret = zoo_create(handle, path, "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
	fprintf(stderr, "CreatebucketsAndChildren() %s, ret=%d\n", path, ret);
	{
		len = snprintf(path, 100, "%s/%s%s", FARM_LIST_PATH, FARM_1_VALUE, ALIVE_NODES_LIST_PATH);
		if(len == 0 ){fprintf(stderr, "snprintf ERROR\n");}
		ret = zoo_create(handle, path, "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
		fprintf(stderr, "CreatebucketsAndChildren() %s, ret=%d\n", path, ret);

		len = snprintf(path, 100, "%s/%s%s", FARM_LIST_PATH, FARM_1_VALUE, COORDINATION);
		ret = zoo_create(handle, path, "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
		fprintf(stderr, "CreatebucketsAndChildren() %s, ret=%d\n", path, ret);
		len = snprintf(path, 100, "%s/%s%s", FARM_LIST_PATH, FARM_1_VALUE, ROLES_PATH);
		ret = zoo_create(handle, path, "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, path_buffer, path_len);
		fprintf(stderr, "CreatebucketsAndChildren() %s, ret=%d\n", path, ret);
	}
}

typedef struct  
{
	uint32_t len;
	char buffer[10000];
}Echo;


Echo* ProcessResponse(char const* buffer, int len, int& exit){
	Echo *pEcho = new Echo; pEcho->len = 0;
	if(strncmp(buffer, "ClearZkTree", len) == 0 ){
		fprintf(stderr, "ClearZkTree()\n");
		ClearZkTree(g_zhandle_);
	}else if (strncmp(buffer, "CreateConfsOnZkTree", len) == 0){
		fprintf(stderr, "CreateConfsOnZkTree()\n");
		CreateConfsOnZkTree(g_zhandle_);
	}else if (strncmp(buffer, "CreateNodesAndChildren", len) == 0){
		fprintf(stderr, "CreateNodesAndChildren()\n");
		CreateNodesAndChildren(g_zhandle_);
	}else if (strncmp(buffer, "CreatebucketsAndChildren", len) == 0){
		fprintf(stderr, "CreatebucketsAndChildren()\n");
		CreatebucketsAndChildren(g_zhandle_);
	}else if (strncmp(buffer, "CreatefarmsAndChildren", len) == 0){
		fprintf(stderr, "CreatefarmsAndChildren()\n");
		CreatefarmsAndChildren(g_zhandle_);
	}else if(strncmp(buffer, "exit", len) == 0 ){
		fprintf(stderr, "exit()\n");
		exit = 1;
	}else if(len == sizeof(ZkOptBody)){
		ZkOptBody* zkoptbody = (ZkOptBody*)buffer;
		if (zkoptbody->opt_type == (int32_t)ksetOptCom){
			std::string path(zkoptbody->path, zkoptbody->path_len);
			std::string value(zkoptbody->value, zkoptbody->value_len);
			zoo_set(g_zhandle_, path.data(), zkoptbody->value, zkoptbody->value_len, -1);
			fprintf(stderr, "zoo_set(), path=%s, value=%s\n", path.data(), value.data());
		}else if(zkoptbody->opt_type == (int32_t)kdeleteOptCom){
			std::string path(zkoptbody->path, zkoptbody->path_len);
			zoo_delete(g_zhandle_, path.data(), -1);
			fprintf(stderr, "zoo_delete(), path=%s\n", path.data());
		}else if(zkoptbody->opt_type == (int32_t)kcreateOptCom){
			char path_buffer[100]; int buffer_len = 100;
			std::string path(zkoptbody->path, zkoptbody->path_len);
			std::string value(zkoptbody->value, zkoptbody->value_len);
			zoo_create(g_zhandle_, path.data(), value.data(), value.size(), &ZOO_OPEN_ACL_UNSAFE, zkoptbody->flags, path_buffer, buffer_len);
			fprintf(stderr, "zoo_create(), path=%s, flags%u\n", path.data(), zkoptbody->flags);
			pEcho->len = strlen(path_buffer);
			strcpy(pEcho->buffer, path_buffer);
			fprintf(stderr, "pEcho->len=%d, create path=%s, pEcho->buffer=%s\n", pEcho->len, path_buffer, pEcho->buffer);
		}
	}

	return pEcho;
}

// test  if 
int main(int argc, char **argv){


    google::ParseCommandLineFlags(&argc, &argv, false);
	
	g_zhandle_ = zookeeper_init(FLAGS_host_port.data(), NULL, 20000, 0, NULL, 0);
	
	if(g_zhandle_ == NULL){
		fprintf(stderr, "zk init ERROR");
		return 1;
	}
	
	
	int socket = CreateUdpSocket(station_socket_port);
	if(socket < 1){
		fprintf(stderr, "create socket ERROR port=%d", station_socket_port);
		return socket;
	}
	
	int nRecvLength = 0;
	int iRet=0;
	struct sockaddr_in fromAddress;
	socklen_t iAddressLen = sizeof(struct sockaddr_in);

	struct timeval tv;
	gettimeofday(&tv, NULL);
	int start_seconds = tv.tv_sec;
	int cur_seconds   = tv.tv_sec;
	while(1){
			
		gettimeofday(&tv, NULL);
		cur_seconds   = tv.tv_sec;
		if (cur_seconds - start_seconds > 180){
			fprintf(stderr, "cur_seconds=%d, start_seconds=%d   break while\n", cur_seconds, start_seconds);
			break;
		}
		
		fd_set fds;
		struct timeval tv;
		FD_ZERO(&fds);
		FD_SET(socket, &fds);
		tv.tv_sec=0;
		tv.tv_usec=10000000; // 默认1s
		iRet = select(socket+1,&fds,NULL,NULL,&tv);
		int exit = 0;
		if(iRet> 0 ){	
			nRecvLength = recvfrom(socket, g_buffer, g_buffer_len, 0, (struct sockaddr*)&fromAddress, &iAddressLen);
			fprintf(stderr, "recvfrom bufferlen=%d\n", nRecvLength);
			Echo* pEcho = ProcessResponse(g_buffer, nRecvLength, exit);

			strcpy(g_send_buffer, "OK!!!");
			int skip = strlen("OK!!!");
			uint32_t v = htonl(pEcho->len);
			memcpy(g_send_buffer+ skip, (void*)&v, sizeof(pEcho->len));
			if (pEcho->len <=10000 )
			{
				memcpy(g_send_buffer+ skip + sizeof(pEcho->len), pEcho->buffer, pEcho->len);
			}
			SendUDP(socket, &fromAddress, g_send_buffer, strlen("OK!!!") + sizeof(pEcho->len) + pEcho->len);
			fprintf(stderr, "pEcho->len=%d\n", pEcho->len);
			if(exit == 1){
				delete pEcho;
				break;
			}
			delete pEcho;
		}
		fprintf(stderr, "select() timeout  10s\n");
	}

	zookeeper_close(g_zhandle_);
	fprintf(stderr, "byebye!!!\n");


	return 0;
}


