
#include <stdint.h>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>		//using singlethread zk
#include "../configure.h"


#include "ut_common.h"
#include "ut_eventlist.h"
#include "ut_configure.h"
#include "ut_keeper.h"



DEFINE_string(host_port, "10.16.28.11:2182", "zookeeper host port");


using namespace ldd::courier;



struct sockaddr_in zkStationAddr;
int						ut_socket_port = 30100;
int						client_socket  = 0;

#define TEST_R(t, n) void MyTest_##t##_##n() 

TEST(Configure, GetRoles0){
	const int 			send_buffer_len = 10000;
	char				send_buffer[send_buffer_len];

	const int 			recv_buffer_len = 10000;
	char				recv_buffer[send_buffer_len];
	InitializeZkTree(client_socket, zkStationAddr);
	usleep(1000000);

	/************************************************************************/
	/* testing start                                                       */
	/************************************************************************/

	 int timeout=400;  const std::string root_path = "";
	Configure*  _configure =	NULL;
	int ret = 0;	Options option; option.timeout = timeout;
	ret = Configure::Init(option, std::string(), FLAGS_host_port,root_path, &_configure);
	ASSERT_EQ(ret, 0);
	
	int id = 0;
	std::string name_space = NAME_SPACE_1;
	ret = _configure->GetNameSpaceID(name_space, &id);
	EXPECT_EQ(ret, kCourierOk);
	EXPECT_EQ(id, 1);

	name_space = NAME_SPACE_2;
	ret = _configure->GetNameSpaceID(name_space, &id);
	EXPECT_EQ(ret, kCourierOk);
	EXPECT_EQ(id, 2);

	name_space = "not found";
	ret = _configure->GetNameSpaceID(name_space, &id);
	EXPECT_EQ(ret, kNotFound);

	{
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(ksetOptCom);
		zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%d", 333);
		zkoptbody.path_len = snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s/%s", NAME_SPACE_LIST_PATH, NAME_SPACE_2);

		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
		usleep(500000);
	}

	name_space = NAME_SPACE_2;
	ret = _configure->GetNameSpaceID(name_space, &id);
	EXPECT_EQ(ret, kCourierOk);
	EXPECT_EQ(id, 333);

	{
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(ksetOptCom);
		zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%d", 2);
		zkoptbody.path_len = snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s/%s", NAME_SPACE_LIST_PATH, NAME_SPACE_2);

		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
		usleep(500000);
	}

	name_space = NAME_SPACE_2;
	ret = _configure->GetNameSpaceID(name_space, &id);
	EXPECT_EQ(ret, kCourierOk);
	EXPECT_EQ(id, 2);

	{	// change value of BUCKET_2 node  bucket_id == 2
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(kdeleteOptCom);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s/%s", NAME_SPACE_LIST_PATH, NAME_SPACE_2);

		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}

	sleep(1);
	name_space = NAME_SPACE_2;
	ret = _configure->GetNameSpaceID(name_space, &id);
	EXPECT_EQ(ret, kNotFound);

	Roles roles;
	int farm_id = atoi(FARM_0_VALUE);
	ret = _configure->GetRoles(farm_id, &roles);
	DLOG(INFO)<<"master1 :"<<roles.master.data();


	_configure->GetRoles(farm_id, &roles);
	DLOG(INFO)<<"master1-1:"<<roles.master.data();

	{	// create roles
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(kcreateOptCom);	zkoptbody.flags = ZOO_SEQUENCE;
		char test[] = "{  \"master\" : [\"0.0.0.0:4545\"], \"slave\" : [\"\"], \"follower\" : [\"\"]}";
		zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%s", test);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s/%s%s%s", FARM_LIST_PATH, FARM_0_VALUE, ROLES_PATH, VERSION_PATH);

		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}
	usleep(1000000);

	ret = _configure->GetRoles(farm_id, &roles);
	DLOG(INFO)<<"master2 :"<<roles.master.data();
	
	EXPECT_STREQ(roles.master.data(), "0.0.0.0:4545");

	ASSERT_EQ(ret, 0);
	delete _configure;
}


TEST(Configure_Keeper, InitializeBaseConfError){

	const int 			send_buffer_len = 10000;
	char				send_buffer[send_buffer_len];

	const int 			recv_buffer_len = 10000;
	char				recv_buffer[send_buffer_len];
	InitializeZkTree(client_socket, zkStationAddr);
	usleep(1000000);

	/************************************************************************/
	/* testing start                                                       */
	/************************************************************************/

	int timeout=400;  const std::string root_path = "";
	Configure*  _configure =	NULL;		Keeper* _keeper;
	int ret = 0;	Options option; option.timeout = timeout;
	ret = Configure::Init(option, std::string(), FLAGS_host_port,root_path, &_configure);
	ASSERT_EQ(ret , 0);
	int bucket_id = 999;   int farm_id=-1;
	ret= _configure->FindFarmIdInBucketList(bucket_id, &farm_id);
	ASSERT_EQ(ret, kNotFound);
	ret= _configure->FindFarmIdInBucketList(0, &farm_id);
	ASSERT_EQ(farm_id, atoi(FARM_0_VALUE));
	ASSERT_EQ(ret, kCourierOk);

	ret= _configure->FindFarmIdInBucketList(0, NULL);
	ASSERT_EQ(ret, kInvalidArgument);

	delete _configure;

	ret = Keeper::Init(option, FLAGS_host_port, HOST_PORT0, root_path, &_keeper);
	ASSERT_EQ(ret , 0);
	delete _keeper;

	{//delete bucket node 
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(kdeleteOptCom);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, BUCKET_NODE_PATH);

		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}
	ret = Configure::Init(option, std::string(), FLAGS_host_port,root_path, &_configure);
	ASSERT_EQ(ret, kInvalidBaseParamInZk);
	bucket_id = 999;
	ret= _configure->FindFarmIdInBucketList(bucket_id, &farm_id);
	ASSERT_EQ(ret, kInvalidBaseParamInZk);
	ret= _configure->FindFarmIdInBucketList(0, NULL);
	ASSERT_EQ(ret, kInvalidBaseParamInZk);
	delete _configure;


	ret = Keeper::Init(option, FLAGS_host_port, HOST_PORT0, root_path, &_keeper);
	ASSERT_EQ(ret , kInvalidBaseParamInZk);
	delete _keeper;


	{	//create bucket node 
		ZkOptBody zkoptbody;
		zkoptbody.opt_type  = int8_t(kcreateOptCom);
		zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%s", BUCKET_NODE_PATH__VALUE);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, BUCKET_NODE_PATH);
		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}
	ret = Configure::Init(option, std::string(), FLAGS_host_port,root_path, &_configure);
	ASSERT_EQ(ret, 0);
	std::vector<std::string> node_list;
	ret = _configure->GetNodeList(NULL, &node_list);
	ASSERT_EQ(ret , 0);
	ASSERT_GT((int)node_list.size(), 0);

	ret = _configure->GetNodeList(NULL, NULL);
	ASSERT_EQ(ret , kInvalidArgument);

	delete _configure;
	ret = Keeper::Init(option, FLAGS_host_port, HOST_PORT0, root_path, &_keeper);
	ASSERT_EQ(ret , 0);
	delete _keeper;

	{		//delete bin log node
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(kdeleteOptCom);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, BIN_LOG_PATH);

		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}
	ret = Configure::Init(option, std::string(), FLAGS_host_port,root_path, &_configure);
	ASSERT_EQ(ret, kInvalidBaseParamInZk);
	Roles roles;		farm_id = -1;
	ret = _configure->GetRoles(farm_id, &roles);
	ASSERT_EQ(ret, kInvalidBaseParamInZk);

	ret = _configure->GetNodeList(NULL, NULL);
	ASSERT_EQ(ret, kInvalidBaseParamInZk);
	delete _configure;

	ret = Keeper::Init(option, FLAGS_host_port, HOST_PORT0, root_path, &_keeper);
	ASSERT_EQ(ret , kInvalidBaseParamInZk);
	delete _keeper;

	{	//create bin log node
		ZkOptBody zkoptbody;
		zkoptbody.opt_type  = int8_t(kcreateOptCom);
		zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%s", BIN_LOG_PATH__VALUE);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, BIN_LOG_PATH);
		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}

	{	// create roles
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(kcreateOptCom);	zkoptbody.flags = ZOO_SEQUENCE;
		char test[] = "{  \"master\" : [\"0.0.0.0:78787\"], \"slave\" : [\"\"], \"follower\" : [\"\"]}";
		zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%s", test);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s/%s%s%s", FARM_LIST_PATH, FARM_0_VALUE, ROLES_PATH, VERSION_PATH);

		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}
	usleep(100000);

	ret = Configure::Init(option, std::string(), FLAGS_host_port,root_path, &_configure);
	ASSERT_EQ(ret, 0);
	farm_id = atoi("-1test");
	ret = _configure->GetRoles(farm_id, &roles);
	ASSERT_EQ(ret, kNotFound);
	ret = _configure->GetRoles(farm_id, NULL);
	ASSERT_EQ(ret, kInvalidArgument);		
	farm_id		=  atoi(FARM_0_VALUE);
	ret = _configure->GetRoles(farm_id, &roles);
	ASSERT_EQ(ret, kCourierOk);		
	delete _configure;

	ret = Keeper::Init(option, FLAGS_host_port, HOST_PORT0, root_path, &_keeper);
	ASSERT_EQ(ret , 0);
	delete _keeper;

	{		//delete engine node
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(kdeleteOptCom);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, ENGINE_NODE_PATH);

		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}
	ret = Configure::Init(option, std::string(), FLAGS_host_port,root_path, &_configure);
	ASSERT_EQ(ret, kInvalidBaseParamInZk);
	delete _configure;
	ret = Keeper::Init(option, FLAGS_host_port, HOST_PORT0, root_path, &_keeper);
	ASSERT_EQ(ret , kInvalidBaseParamInZk);
	delete _keeper;

	{	//create bin log node
		ZkOptBody zkoptbody;
		zkoptbody.opt_type  = int8_t(kcreateOptCom);
		zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%s", ENGINE_NODE_PATH__VALUE);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, ENGINE_NODE_PATH);
		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}
	ret = Configure::Init(option, std::string(), FLAGS_host_port,root_path, &_configure);
	ASSERT_EQ(ret, 0);
	delete _configure;
	ret = Keeper::Init(option, FLAGS_host_port, HOST_PORT0, root_path, &_keeper);
	ASSERT_EQ(ret , 0);
	delete _keeper;

	{		//delete replica node
		ZkOptBody zkoptbody;
		zkoptbody.opt_type = int8_t(kdeleteOptCom);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, REPLICA_NODE_PATH);

		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}
	ret = Configure::Init(option, std::string(), FLAGS_host_port,root_path, &_configure);
	ASSERT_EQ(ret, kInvalidBaseParamInZk);
	delete _configure;
	ret = Keeper::Init(option, FLAGS_host_port, HOST_PORT0,root_path, &_keeper);
	ASSERT_EQ(ret , kInvalidBaseParamInZk);
	delete _keeper;

	{	//create replica node
		ZkOptBody zkoptbody;
		zkoptbody.opt_type  = int8_t(kcreateOptCom);
		zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%s", REPLICA_NODE_PATH__VALUE);
		zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, REPLICA_NODE_PATH);
		memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
		sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
		ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
	}
	ret = Configure::Init(option, std::string(), FLAGS_host_port,root_path, &_configure);
	ASSERT_EQ(ret, 0);
	delete _configure;
	ret = Keeper::Init(option, FLAGS_host_port,HOST_PORT0, root_path, &_keeper);
	ASSERT_EQ(ret , 0);
	delete _keeper;
}


#undef  TEST_R

// test  if 
int main(int argc, char **argv){
	const int 		send_buffer_len = 10000;
	char				send_buffer[send_buffer_len];

	const int 		recv_buffer_len = 10000;
	char				recv_buffer[send_buffer_len];

	int ret = 0;
	google::InitGoogleLogging(argv[0]);		// ./your_application --log_dir=.
    google::ParseCommandLineFlags(&argc, &argv, false);
	//google::SetLogDestination(google::INFO,"./");

	client_socket = CreateUdpSocket(ut_socket_port);
	if(client_socket < 1){
		fprintf(stderr, "create socket error port=%d", ut_socket_port);
		return -1;
	}
	SetAddress(zkStationAddr, "0.0.0.0", station_socket_port);

	testing::InitGoogleTest(&argc, argv);

	ret = RUN_ALL_TESTS();


	google::FlushLogFiles(google::INFO);
	google::FlushLogFilesUnsafe(google::INFO);
	//ClearZkTree(client_socket, zkStationAddr);   // bug!!!!!

	// should be the last string command 
	int recv_len = 0 ;
	strcpy(send_buffer, "exit");
	recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, strlen("exit"), recv_buffer, recv_buffer_len);
	if (strncmp(recv_buffer, "OK!!!", strlen("OK!!!")) != 0){
		fprintf(stderr, "send exit error \n");
	}


	fprintf(stderr, "BYE BYE !!!!\n");

	LOG(INFO)<<"BYE BYE !!!!";
	google::ShutdownGoogleLogging();

	return 0;
}
