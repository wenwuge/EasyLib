
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "proto.h"
#include "store_proto.h"

using namespace ldd::protocol;
using namespace ldd::util;


#define TEST_R(t, n) void MyTest_##t##_##n() 

TEST(proto, GetMessage){
	GetMessage gm;
	std::string ns = "GetMessage";
	gm.name_space_ = ns;
	char* s = new char[100];
	int len = snprintf(s, 100, "%d", 22);
	Slice sss(s, len);
	gm.key_ = sss;
	int size = 	gm.MaxSize();
	char* buffer = new char[size];
	size_t real_len = gm.SerializeTo(buffer);

	GetMessage exp;
	bool ret = exp.ParseFrom(buffer, real_len);
	ASSERT_TRUE(ret);
	EXPECT_TRUE(strncmp(exp.name_space_.data(), gm.name_space_.data(),gm.name_space_.size() ) == 0);

	const char* a = exp.key_.data();
	const char* b = gm.key_.data();
	EXPECT_TRUE(strncmp(a, b, gm.key_.size() ) == 0);
	delete[] gm.key_.data();
	delete [] buffer;

	{
		GetMessage gm;
		std::string ns = "GetMessage";
		gm.name_space_ = ns;
		Slice sss = "test;";
		gm.key_ = sss;
		int size = 	gm.MaxSize();
		char* buffer = new char[size];
		size_t len2 = gm.SerializeTo(buffer);

		GetMessage respon;
		respon.ParseFrom(buffer, len2);

		ASSERT_TRUE(ret);
		delete buffer;
	}
}

TEST(proto, CheckMessage){
	CheckMessage cm;
	std::string ns = "CheckMessage";
	cm.name_space_ = ns;
	char* s = new char[100];
	int len = snprintf(s, 100, "%d", 10000);
	Slice sss(s, len);
	cm.key_=sss;
	int size = 	cm.MaxSize();
	char* buffer = new char[size];
	size_t real_len = cm.SerializeTo(buffer);
	ASSERT_GT((int)real_len, 0);

	CheckMessage exp;
	bool ret = exp.ParseFrom(buffer, real_len);
	ASSERT_TRUE(ret);
	EXPECT_TRUE(strncmp(exp.name_space_.data(), cm.name_space_.data(),cm.name_space_.size() ) == 0);

	const char* a = exp.key_.data();
	const char* b = cm.key_.data();
	EXPECT_TRUE(strncmp(a, b, cm.key_.size() ) == 0);
	delete[] cm.key_.data();
	delete [] buffer;


	{
		CheckMessage cm;
		std::string ns = "CheckMessage";
		cm.name_space_ = ns;
		Slice sss = "test";
		cm.key_=sss;
		int size = 	cm.MaxSize();
		char* buffer = new char[size];
		size_t real_len = cm.SerializeTo(buffer);
		ASSERT_GT((int)real_len, 0);
		delete buffer;
	}
}

TEST(proto, ListMessage)
{
	ListMessage lm;
	lm.name_space_ = "ListMessage";
	lm.key_				= "ListMessageS_key" ;
	lm.u32limit_ = 43543546u;
	lm.u8position_	 = 1u;

	int len = lm.MaxSize();
	char* buf = new char[len];
	size_t real_len = lm.SerializeTo(buf);
	ASSERT_GT((int)real_len, 0);

	ListMessage expect;
	bool ret = expect.ParseFrom(buf, real_len);
	ASSERT_TRUE(ret);

	ASSERT_TRUE(strncmp(expect.name_space_.data(), lm.name_space_.data(), lm.name_space_.size())==0);
	ASSERT_TRUE(strncmp(expect.key_.data(), lm.key_.data(), lm.key_.size())==0);
	ASSERT_EQ(expect.u32limit_,	lm.u32limit_);
	ASSERT_EQ(expect.u8position_, lm.u8position_);
	delete buf;
}


TEST(proto, AtomicIncrMessage)
{
	AtomicIncrMessage paim;
	paim.name_space_ = "ProxyAtomicIncrMessage";
	paim.key_		   = "ProxyAtomicIncrMessageKey";
	paim.s32operand_	= 892839289;	
	paim.s32initial_	= 10000;			//  (u32InitialLen)
	paim.u64ttl_		= 32329389u;

	int len = paim.MaxSize();
	char* buf = new char[len];
	size_t real_len = paim.SerializeTo(buf);
	ASSERT_GT((int)real_len, 0);

	AtomicIncrMessage expect;
	expect.ParseFrom(buf, real_len);

	ASSERT_TRUE(strncmp(expect.name_space_.data(), paim.name_space_.data(), paim.name_space_.size())==0);
	ASSERT_TRUE(strncmp(expect.key_.data(), paim.key_.data(), paim.key_.size())==0);
	ASSERT_EQ(expect.s32operand_, paim.s32operand_);
	ASSERT_EQ(expect.s32initial_, paim.s32initial_);

	ASSERT_EQ(expect.u64ttl_,	paim.u64ttl_);
	delete buf;
}

TEST(proto, AtomicAppendMessage)
{
	AtomicAppendMessage paim;
	paim.name_space_	= "ProxyAtomicAppendMessage";
	paim.key_			= "ProxyAtomicAppendMessageKey";
	paim.u8position_		=  89;	
	paim.content_			=  "slice_";			// (u32InitialLen)
	paim.u64ttl_		= 43444444lu;

	int len = paim.MaxSize();
	char* buf = new char[len];
	size_t real_len = paim.SerializeTo(buf);
	ASSERT_GT((int)real_len, 0);

	AtomicAppendMessage expect;
	expect.ParseFrom(buf, len);

	ASSERT_TRUE(strncmp(expect.name_space_.data(), paim.name_space_.data(), paim.name_space_.size())==0);
	ASSERT_TRUE(strncmp(expect.key_.data(), paim.key_.data(), paim.key_.size())==0);
	ASSERT_EQ(expect.u8position_, paim.u8position_);
	ASSERT_TRUE(strncmp(expect.content_.data(), paim.content_.data(), paim.content_.size())==0);

	ASSERT_EQ(expect.u64ttl_,	paim.u64ttl_);
	delete buf;
}

TEST(proto, MutateMessage)
{
	MutateMessage gm;
	std::string ns		= "MutateMessage";
	gm.name_space_		= ns;
	stMutation key_value_;
	gm.mutation_.key					= "MutateMessage_key";
	gm.mutation_.opt_type				= 22%2+1;
	gm.mutation_.has_expected_version	= 22%2;
	gm.mutation_.expected_version		= 111111111u;
	gm.mutation_.has_expected_val		= 22%2;
	gm.mutation_.expected_value			= "MutateMessage_expect_value";

	gm.mutation_.value					= "MutateMessage_value";
	gm.mutation_.has_ttl				= 22%2;
	gm.mutation_.ttl					= 243434u;
	int size = 	gm.MaxSize();
	char* buffer = new char[size];
	size_t real_len = gm.SerializeTo(buffer);
	ASSERT_GT((int)real_len, 0);

	MutateMessage exp;
	bool ret = exp.ParseFrom(buffer, real_len);
	ASSERT_TRUE(ret);
	EXPECT_TRUE(strncmp(exp.name_space_.data(), gm.name_space_.data(),gm.name_space_.size() ) == 0);

	stMutation& expect = exp.mutation_;
	stMutation& right  = gm.mutation_;
	EXPECT_TRUE(strncmp(expect.key.data(), right.key.data(), expect.key.size() ) == 0);
	ASSERT_EQ(expect.opt_type , right.opt_type);
	ASSERT_EQ(expect.has_expected_version , right.has_expected_version);
	if (expect.has_expected_version == 1){
		ASSERT_EQ(expect.expected_version , right.expected_version);
	}

	ASSERT_EQ(expect.has_expected_val , right.has_expected_val);

	if (expect.has_expected_val == 1){
		EXPECT_TRUE(strncmp(expect.expected_value.data(), right.expected_value.data(), expect.expected_value.size() ) == 0);
	}

	if (expect.opt_type == 1){
		EXPECT_TRUE(strncmp(expect.value.data(), right.value.data(), expect.value.size() ) == 0);		
		ASSERT_EQ(expect.has_ttl , right.has_ttl);
		if (expect.has_ttl == 1){
			ASSERT_EQ(expect.ttl , right.ttl);
		}
	}
	delete [] buffer;
}





#undef  TEST_R


