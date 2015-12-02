
#include <stdint.h>
#include <glog/logging.h>
#include <gtest/gtest.h>		

#include "proto.h"
#include "store_proto.h"


#include "ut_proto.h"
#include "ut_msg_proto.h"
#include "ut_response.h"

using namespace ldd::protocol;

#define TEST_R(t, n) void MyTest_##t##_##n() 



#undef  TEST_R


// test  if 
int main(int argc, char **argv){
	int ret = 0;
	google::InitGoogleLogging(argv[0]);		// ./your_application --log_dir=.
	google::SetLogDestination(google::INFO,"./");

	testing::InitGoogleTest(&argc, argv);

	ret = RUN_ALL_TESTS();


	google::FlushLogFiles(google::INFO);
	google::FlushLogFilesUnsafe(google::INFO);

	fprintf(stderr, "BYE BYE !!!!\n");

	LOG(INFO)<<"BYE BYE !!!!";
	google::ShutdownGoogleLogging();

	return 0;
}
