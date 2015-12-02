
#include <glog/logging.h>
#include <gtest/gtest.h>

#include "response.h"

using namespace ldd::protocol;
using namespace ldd::util;

#define TEST_R(t, n) void MyTest_##t##_##n() 


TEST(response, GetOrListResponse)
{
	GetOrListResponse paim;

	paim.key_		= "GetOrListResponse_key";
	paim.value_		= "GetOrListResponse_value";
	paim.s64Version_	= 98392839;
	paim.u64ttl_	     = 983839;

	int len = paim.MaxSize();
	char* buf = new char[len];
	size_t real_len = paim.SerializeTo(buf);
	ASSERT_GT((int)real_len, 0);

	GetOrListResponse expect;
	bool ret = expect.ParseFrom(buf, real_len);
	ASSERT_TRUE(ret);
	ASSERT_TRUE(strncmp(expect.key_.data(), paim.key_.data(), paim.key_.size())==0);	
	ASSERT_EQ(expect.s64Version_, paim.s64Version_);
	ASSERT_EQ(expect.u64ttl_, paim.u64ttl_);
	ASSERT_TRUE(strncmp(expect.value_.data(), paim.value_.data(), paim.value_.size())==0);	
	delete buf;
}

TEST(response, CheckResponse)
{
	CheckResponse paim;

	paim.key_		= "CheckResponse_key";
	paim.s64Version_	= 21222222222;

	int len = paim.MaxSize();
	char* buf = new char[len];
	size_t real_len = paim.SerializeTo(buf);
	ASSERT_GT((int)real_len, 0);

	CheckResponse expect;
	bool ret = expect.ParseFrom(buf, real_len);
	ASSERT_TRUE(ret);
	ASSERT_TRUE(strncmp(expect.key_.data(), paim.key_.data(), paim.key_.size())==0);	
	ASSERT_EQ(expect.s64Version_, paim.s64Version_);
	delete buf;
}

TEST(response, MutateResponse)
{
	MutateResponse paim;

	paim.s64Version_	= 212123244;

	int len = paim.MaxSize();
	char* buf = new char[len];
	size_t real_len = paim.SerializeTo(buf);
	ASSERT_GT((int)real_len, 0);

	MutateResponse expect;
	bool ret = expect.ParseFrom(buf, real_len);
	ASSERT_TRUE(ret);
	ASSERT_EQ(expect.s64Version_, paim.s64Version_);
	delete buf;
}

TEST(response, CasResponse)
{
	CasResponse paim;
	paim.s64Version_   = 32793729;
	paim.key_			= "atkjsdk";
	paim.value_			= "fdsul";

	int len = paim.MaxSize();
	char* buf = new char[len];
	size_t real_len = paim.SerializeTo(buf);
	ASSERT_GT((int)real_len, 0);

	CasResponse expect;
	bool ret = expect.ParseFrom(buf, len);
	ASSERT_TRUE(ret);
	ASSERT_EQ(expect.s64Version_, paim.s64Version_);
	ASSERT_TRUE(strncmp(expect.key_.data(), paim.key_.data(), expect.key_.size())==0);	
	ASSERT_TRUE(strncmp(expect.value_.data(), paim.value_.data(), expect.value_.size())==0);	
	delete buf;
}

TEST(response, GetVersionResponse)
{
	GetVersionResponse paim;
	paim.s64Version_   = 32793729;
	paim.home_host_port_			= "0.0.0.0:1122";
	paim.dest_host_port_			= "0.0.0.0:1122234";

	int len = paim.MaxSize();
	char* buf = new char[len];
	size_t real_len = paim.SerializeTo(buf);
	ASSERT_GT((int)real_len, 0);

	GetVersionResponse expect;
	bool ret = expect.ParseFrom(buf, len);
	ASSERT_TRUE(ret);
	ASSERT_EQ(expect.s64Version_, paim.s64Version_);
	ASSERT_TRUE(strncmp(expect.home_host_port_.data(), paim.home_host_port_.data(), expect.home_host_port_.size())==0);	
	ASSERT_TRUE(strncmp(expect.dest_host_port_.data(), paim.dest_host_port_.data(), expect.dest_host_port_.size())==0);	
	delete buf;
}



#undef  TEST_R


