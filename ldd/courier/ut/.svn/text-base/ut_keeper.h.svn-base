

#ifndef _LDD_UT_KEEPER_H_
#define _LDD_UT_KEEPER_H_

#include "ut_common.h"
#include "../keeper.h"
#include <zookeeper/zookeeper.h>
#include "../options.h"


using namespace ldd::courier;

#define TEST_R(t, n) void MyTest_##t##_##n() 


DECLARE_string(host_port);

TEST(Keeper, initialize){

	const int 		send_buffer_len = 10000;
	char				send_buffer[send_buffer_len];
 
	const int 		recv_buffer_len = 10000;
	char				recv_buffer[send_buffer_len];

	/************************************************************************/
	/* testing courier                                                       */
	/************************************************************************/
	InitializeZkTree(client_socket, zkStationAddr);

	int timeout=400;  const std::string root_path = "";
	Keeper* keeper_ =	NULL;
	const std::string home_host_port = HOST_PORT0;
	int ret = 0;	Options option; option.timeout = timeout;
	ret = Keeper::Init(option, FLAGS_host_port, home_host_port, root_path, &keeper_);
	ASSERT_EQ(ret, 0);
	LOG(INFO)<<"initialize successfully";


	int bucket_size = 0;  int replicaSize = 0;  std::string engine_name; int MaxBinLogFile = 0;
	ret =  keeper_->GetBucketSize(&bucket_size);
	ret =  keeper_->GetReplicaSize(&replicaSize) ;
	ret =  keeper_->GetStoreEngine(&engine_name) ;
	ret =  keeper_->GetMaxBinLogFile(&MaxBinLogFile) ;
	EXPECT_EQ(bucket_size, atoi(BUCKET_NODE_PATH__VALUE));
	EXPECT_EQ(replicaSize, atoi(REPLICA_NODE_PATH__VALUE));
	EXPECT_EQ(MaxBinLogFile, atoi(BIN_LOG_PATH__VALUE));
	EXPECT_STREQ(engine_name.data(), ENGINE_NODE_PATH__VALUE);
	EXPECT_EQ(bucket_size, atoi(BUCKET_NODE_PATH__VALUE));

	int32_t replicaSize22 = 10;  int32_t MaxBinLogFile22 = 55555;  int32_t  bucket_size22 = 2;
	std::string engine_name22 = "gfs";
	 int recv_len = 0;
	{
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(ksetOptCom);
		zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%d", replicaSize22);
		zkoptbody.path_len = snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, REPLICA_NODE_PATH);

		memcpy(send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}

	{
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(ksetOptCom);
		zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%d", MaxBinLogFile22);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, BIN_LOG_PATH);

		memcpy(send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}

	{
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(ksetOptCom);
		zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%d", bucket_size22);
		zkoptbody.path_len  = snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, BUCKET_NODE_PATH);

		memcpy(send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}

	{
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(ksetOptCom);
		zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%s", engine_name22.data());
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, ENGINE_NODE_PATH);

		memcpy(send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}

	usleep(10000);  //sleep(seconds)  // after change zk tree, should wait 10 ms for ut test

	keeper_->GetBucketSize(&bucket_size);
	keeper_->GetReplicaSize(&replicaSize) ;
	keeper_->GetStoreEngine(&engine_name) ;
	keeper_->GetMaxBinLogFile(&MaxBinLogFile) ;

	EXPECT_EQ(replicaSize, atoi(REPLICA_NODE_PATH__VALUE));
	EXPECT_EQ(MaxBinLogFile, MaxBinLogFile22);
	EXPECT_STREQ(engine_name.data(),ENGINE_NODE_PATH__VALUE);
	EXPECT_EQ(bucket_size, atoi(BUCKET_NODE_PATH__VALUE));

	//ClearZkTree(client_socket, zkStationAddr);  
	
	DLOG(INFO)<<"before delete";
	google::FlushLogFiles(google::INFO);
	google::FlushLogFilesUnsafe(google::INFO);

	delete keeper_;   // need close thread, delete reactor
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

}


//for StartLeaderElection0
void AliveNodeChangeCB(int ret, NodeChangedType type, const std::string& node){
	if (type == kCreateNode){
		if(strcmp(node.data(), HOST_PORT2) != 0 ){
			fprintf(stderr, "not equal, data=%s, test1=%s\n", node.data(), HOST_PORT2);
			exit(11);
		}
		fprintf(stderr, "kCreateNode, data=%s\n", node.data());
		DLOG(INFO)<<"kCreateNode, data=%s"<<node.data();
	}else if (type == kDeleteNode){
		if(strcmp(node.data(), HOST_PORT0) == 0 ){
			fprintf(stderr, "kDeleteNode, data=%s\n", node.data());
			DLOG(INFO)<<"kDeleteNode, data=%s"<<node.data();
		}else if(strcmp(node.data(), HOST_PORT2) == 0 ){
			fprintf(stderr, "kDeleteNode, data=%s\n", node.data());
			DLOG(INFO)<<"kDeleteNode, data=%s"<<node.data();
		}else{
			fprintf(stderr, "ndata=%s, HOST_PORT0=10.16.28.11:40000|123\n", node.data());
			exit(22);
		}
	}
	return ;
}

void SLECB(int err_code, bool got_leadership){
	DLOG(INFO)<<"I am the leader. got_leadership="<<got_leadership;
	fprintf(stderr, "I am the leader, got_leadership=%d, err_code=%d\n", got_leadership, err_code);
	return;
}

TEST(Keeper, StartLeaderElection0){
	const int 			send_buffer_len = 10000;
	char				send_buffer[send_buffer_len];

	const int 			recv_buffer_len = 10000;
	char				recv_buffer[send_buffer_len];

	/************************************************************************/
	/* testing courier                                                       */
	/************************************************************************/
	InitializeZkTree(client_socket, zkStationAddr);

	int timeout=400;  const std::string root_path = "";
	Keeper* keeper_ =	NULL;
	int ret = 0;	Options option; option.timeout = timeout;
	ret = Keeper::Init(option, FLAGS_host_port, HOST_PORT0, root_path, &keeper_);
	ASSERT_EQ(ret, 0);
	LOG(INFO)<<"initialize successfully";

	keeper_->StartLeaderElection(SLECB);
	usleep(2000000);
	  
	std::vector<std::string> server_list ;
	
	keeper_->GetAliveNodes(AliveNodeChangeCB, &server_list);
	EXPECT_EQ((int)server_list.size(), 1);
	if (server_list.size() > 0){
		EXPECT_STREQ(server_list[0].data(), HOST_PORT0);
	}

	int recv_len = 0;
	{	// change value of BUCKET_2 node  bucket_id == 2
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(kdeleteOptCom);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s/%s%s/%s", FARM_LIST_PATH, FARM_0_VALUE
							,ALIVE_NODES_LIST_PATH, HOST_PORT0);

		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}

	//  host_port1
	{	// change value of BUCKET_2 node  bucket_id == 2
		ZkOptBody zkoptbody;	int recv_len=0;
		zkoptbody.opt_type  = int8_t(kcreateOptCom);
		zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%s", "");
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s/%s%s/%s", FARM_LIST_PATH, FARM_0_VALUE
														,ALIVE_NODES_LIST_PATH, HOST_PORT2);
		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}

	{	// change value of BUCKET_2 node  bucket_id == 2
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(kdeleteOptCom);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s/%s%s/%s", FARM_LIST_PATH, FARM_0_VALUE
			,ALIVE_NODES_LIST_PATH, HOST_PORT2);

		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}
	keeper_->GetAliveNodes(NULL, &server_list);
	usleep(1000000);

	delete keeper_;
}

void RolesChangedcb1(int err_code, Roles& roles){

	fprintf(stderr, "RolesChangedcb1() master:%s\n", roles.master.data());
}

TEST(Keeper, StartLeaderElection1){
	const int 			send_buffer_len = 10000;
	char				send_buffer[send_buffer_len];

	const int 			recv_buffer_len = 10000;
	char				recv_buffer[send_buffer_len];

	/************************************************************************/
	/* testing courier                                                       */
	/************************************************************************/
	InitializeZkTree(client_socket, zkStationAddr);

	int timeout=400;  const std::string root_path = "";
	Keeper* keeper_ =	NULL;
	int ret = 0;	Options option; option.timeout = timeout;
	ret = Keeper::Init(option, FLAGS_host_port,HOST_PORT0, root_path, &keeper_);
	ASSERT_EQ(ret, 0);
	LOG(INFO)<<"initialize successfully";
	const char* add_competor = "x-9999999999999999-";
	char path_created[1000]={0};
	//  host_port1
	{	// change value of BUCKET_2 node  bucket_id == 2
		ZkOptBody zkoptbody;	int recv_len=0;
		zkoptbody.opt_type  = int8_t(kcreateOptCom);	zkoptbody.flags = ZOO_EPHEMERAL|ZOO_SEQUENCE;
		fprintf(stderr, "create flags=%u\n", zkoptbody.flags);
		zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%s", "");
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s/%s%s/%s", FARM_LIST_PATH, FARM_0_VALUE
			,COORDINATION, add_competor);
		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
		int j = strlen("OK!!!");
		uint32_t len = *((uint32_t*)(recv_buffer + j)); 
		len = ntohl(len); 
		memcpy(path_created, recv_buffer+j+sizeof(uint32_t), len);
		fprintf(stderr, "path recv_len=%d, created len=%u, path=%s\n", recv_len, len, path_created);
	}
	usleep(1000000);

	DLOG(INFO)<<"create 999999";

	keeper_->StartLeaderElection(SLECB);
	usleep(1000000);

	std::vector<std::string> server_list ;

	keeper_->GetAliveNodes(NULL, &server_list);
	DLOG(INFO)<<"GetOnlineNodes";
	EXPECT_GE((int)server_list.size(), 1);
	bool find = false;
	for (size_t i=0; i<server_list.size(); i++){
		if (strcmp(server_list[i].data(), HOST_PORT0) == 0){
			find = true;
		}
	}
	EXPECT_TRUE(find);

	Roles get_roles2;
	keeper_->GetRoles(RolesChangedcb1, &get_roles2);
	fprintf(stderr, "master-----%s\n", get_roles2.master.data());

	//EXPECT_STREQ(server_list[0].data(), HOST_PORT0);			

	{	// change value of BUCKET_2 node  bucket_id == 2
		ZkOptBody zkoptbody;	int recv_len = 0;
		zkoptbody.opt_type = int8_t(kdeleteOptCom);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s", path_created);

		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
		fprintf(stderr, "kdeleteOptCom, path=%s", path_created);
	}
	DLOG(INFO)<<"delete 999999";

	Roles roles_try;

	char buffer[1000]; int buffer_len = 1000;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	int second = tv.tv_sec;
	buffer_len = snprintf(buffer, buffer_len, "10.16.28.11:%d", second);

	roles_try.master = std::string(buffer, buffer_len);
	roles_try.slaves.push_back("10.16.28.11:10001");
	roles_try.slaves.push_back("10.16.28.11:10002");

	roles_try.followers.push_back("10.16.28.11:10003");
	roles_try.followers.push_back("10.16.28.11:10004");
	roles_try.followers.push_back("10.16.28.11:10005");

	keeper_->AddNewRoles(roles_try);
	usleep(100000);

	Roles get_roles;
	keeper_->GetRoles(RolesChangedcb1, &get_roles);

	fprintf(stderr, "master-----%s\n", get_roles.master.data());

	usleep(1000000);
	google::FlushLogFiles(google::INFO);
	google::FlushLogFilesUnsafe(google::INFO);


	//usleep(1000000000);

	delete keeper_;
}



#undef  TEST_R


#endif  // _LDD_UT_KEEPER_H_


