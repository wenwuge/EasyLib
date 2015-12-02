
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "proto.h"
#include "store_proto.h"

using namespace ldd::protocol;
using namespace ldd::util;

#define TEST_R(t, n) void MyTest_##t##_##n() 


TEST(msg_proto, GetMessageS){
	GetMessageS gm;
	gm.u16Ns_		 = 14552;

	Slice sss = "GetMessageS";
	gm.key_ = sss;
	gm.u16BucketID_	= 200;
	int size = 	gm.MaxSize();
	char* buffer = new char[size];
	size_t real_len = gm.SerializeTo(buffer);
	ASSERT_GT((int)real_len, 0);

	GetMessageS exp;
	bool ret = exp.ParseFrom(buffer, real_len);
	ASSERT_TRUE(ret);
	EXPECT_EQ(exp.u16Ns_, gm.u16Ns_);

	ASSERT_EQ(exp.u16BucketID_, gm.u16BucketID_);
	EXPECT_TRUE(strncmp(exp.key_.data(), gm.key_.data(), gm.key_.size() ) == 0);
	delete [] buffer;

	{
		GetMessageS gm;
		std::string ns = "GetMessage";
		char* s = new char[100];
		int len = snprintf(s, 100, "%d", 200);
		Slice sss(s, len);
		gm.key_ = sss;
		gm.u16BucketID_	= 22;
		int size = 	gm.MaxSize();
		char* buffer = new char[size];
		size_t real_len = gm.SerializeTo(buffer);
		ASSERT_GT((int)real_len, 0);
		delete buffer;
	}
}

TEST(msg_proto, CheckMessageS){
	CheckMessageS gm;
	gm.u16Ns_		 = 14552;

	Slice sss = "CheckMessageS";
	gm.key_ = sss;
	gm.u16BucketID_	= 22;
	int size = 	gm.MaxSize();
	char* buffer = new char[size];
	size_t real_len = gm.SerializeTo(buffer);
	ASSERT_GT((int)real_len, 0);

	CheckMessageS exp;
	bool ret = exp.ParseFrom(buffer, real_len);
	ASSERT_TRUE(ret);
	EXPECT_EQ(exp.u16Ns_, gm.u16Ns_);

	ASSERT_EQ(exp.u16BucketID_, gm.u16BucketID_);
	EXPECT_TRUE(strncmp(exp.key_.data(), gm.key_.data(), gm.key_.size() ) == 0);
	delete [] buffer;

	{
		CheckMessageS gm;
		std::string ns = "GetMessage";
		char* s = new char[100];
		int len = snprintf(s, 100, "%d", 100);
		Slice sss(s, len);
		gm.key_ = sss;
		gm.u16BucketID_	= 22;
		int size = 	gm.MaxSize();
		char* buffer = new char[size];
		size_t real_len = gm.SerializeTo(buffer);
		ASSERT_GT((int)real_len, 0);
		delete buffer;
	}
}

TEST(msg_proto, ListMessageS)
{
	ListMessageS lm;
	lm.u16Ns_			 = 32342;
	lm.key_				= "ListMessageS_key" ;
	lm.u32limit_ = 43543546u;
	lm.u8position_	 = 1u;

	int len = lm.MaxSize();
	char* buf = new char[len];
	size_t real_len = lm.SerializeTo(buf);
	ASSERT_GT((int)real_len, 0);

	ListMessageS expect;
	bool ret = expect.ParseFrom(buf, len);
	ASSERT_TRUE(ret);

	ASSERT_EQ(expect.u16Ns_, lm.u16Ns_);
	EXPECT_TRUE(strncmp(lm.key_.data(), expect.key_.data(), expect.key_.size() ) == 0);
	ASSERT_EQ(expect.u32limit_,	lm.u32limit_);
	ASSERT_EQ(expect.u8position_, lm.u8position_);
	delete buf;
}


TEST(msg_proto, AtomicIncrMessageS)
{
	AtomicIncrMessageS paim;
	paim.u16Ns_			= 5748;
	paim.u16BucketID_	= 9999;
	paim.key_			= "AtomicIncrMessageSKey";
	paim.s32operand_	=  1139289;	
	paim.s32initial_	=  434535;			// (u32InitialLen)
	paim.u64ttl_		=  329389u;
	paim.s64syn_version_= 4242445u;
	paim.u8from_			= 99;

	int len = paim.MaxSize();
	char* buf = new char[len];
	size_t real_len = paim.SerializeTo(buf);
	ASSERT_GT((int)real_len, 0);

	AtomicIncrMessageS expect;
	expect.ParseFrom(buf, len);

	ASSERT_EQ(expect.u16Ns_, paim.u16Ns_);
	ASSERT_EQ(expect.u16BucketID_, paim.u16BucketID_);

	ASSERT_TRUE(strncmp(expect.key_.data(), paim.key_.data(), paim.key_.size())==0);
	ASSERT_EQ(expect.s32operand_, paim.s32operand_);
	ASSERT_EQ(expect.s32initial_, paim.s32initial_);

	ASSERT_EQ(expect.u64ttl_,	paim.u64ttl_);
	ASSERT_EQ(expect.s64syn_version_,	paim.s64syn_version_);
	ASSERT_EQ(expect.u8from_,	paim.u8from_);

	delete buf;
}

TEST(msg_proto, AtomicAppendMessageS)
{
	AtomicAppendMessageS paim;
	paim.u16Ns_	= 0;
	paim.u16BucketID_	= 3;
	paim.key_			= "ProxyAtomicAppendMessageKey";
	paim.u8position_		=  89;	
	paim.content_			=  "slice_";			// (u32InitialLen)
	paim.u64ttl_		= 43444444lu;
	paim.s64syn_version_= 4242445u;
	paim.u8from_			= 99;

	int len = paim.MaxSize();
	char* buf = new char[len];
	size_t real_len = paim.SerializeTo(buf);
	ASSERT_GT((int)real_len, 0);

	AtomicAppendMessageS expect;
	expect.ParseFrom(buf, len);

	ASSERT_EQ(expect.u16Ns_, paim.u16Ns_);
	ASSERT_EQ(expect.u16BucketID_, paim.u16BucketID_);
	ASSERT_TRUE(strncmp(expect.key_.data(), paim.key_.data(), paim.key_.size())==0);
	ASSERT_EQ(expect.u8position_, paim.u8position_);
	ASSERT_TRUE(strncmp(expect.content_.data(), paim.content_.data(), paim.content_.size())==0);

	ASSERT_EQ(expect.u64ttl_,	paim.u64ttl_);
	ASSERT_EQ(expect.s64syn_version_,	paim.s64syn_version_);
	ASSERT_EQ(expect.u8from_,	paim.u8from_);
	delete buf;
}


TEST(msg_proto, MutateMessageS)
{
	MutateMessageS gm;
	gm.u16Ns_			= 47857;

	int i  =999;
	gm.u16BucketID_ = 5798;
	gm.mutation_.key					= "MutateMessage_key";
	gm.mutation_.opt_type				= i%2+1;
	gm.mutation_.has_expected_version	= i%2;
	gm.mutation_.expected_version		= 111111111u;
	gm.mutation_.has_expected_val		= i%2;
	gm.mutation_.expected_value			= "MutateMessage_expect_value";

	gm.mutation_.value					= "MutateMessage_value";
	gm.mutation_.has_ttl				= i%2;
	gm.mutation_.ttl					= 243434u;

	gm.s64syn_version_= 4242445u;
	gm.u8from_			= 99;

	int size = 	gm.MaxSize();
	char* buffer = new char[size];
	size_t real_len = gm.SerializeTo(buffer);
	ASSERT_GT((int)real_len, 0);

	MutateMessageS exp;
	bool ret = exp.ParseFrom(buffer, real_len);
	ASSERT_TRUE(ret);
	EXPECT_EQ(exp.u16Ns_, gm.u16Ns_);

	stMutation& expect = exp.mutation_;
	stMutation& right = gm.mutation_;
	ASSERT_EQ( exp.u16BucketID_, gm.u16BucketID_);
	ASSERT_EQ( exp.s64syn_version_, gm.s64syn_version_);
	ASSERT_EQ( exp.u8from_, gm.u8from_);

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


TEST(msg_proto, GetVersionMessageS){

	GetVersionMessageS gms;
	gms.host_port_ = "0.0.0.0:23456";
	int size = gms.MaxSize();
	char* buffer = new char[size];

	size_t real_len = gms.SerializeTo(buffer);
	ASSERT_GT((int)real_len, 0);

	GetVersionMessageS expect;
	bool ret = expect.ParseFrom(buffer, size);
	ASSERT_TRUE(ret);

	EXPECT_TRUE(strncmp(expect.host_port_.data(), gms.host_port_.data(), expect.host_port_.size() ) == 0);
	delete buffer;
}

TEST(msg_proto, PrepareMessageS){

	PrepareMessageS gms;
	gms.home_port_		= "0.0.0.0:23456";
	gms.dest_host_port_	= "0.0.0.0:222222";

	int size = gms.MaxSize();
	char* buffer = new char[size];

	size_t real_len = gms.SerializeTo(buffer);
	ASSERT_GT((int)real_len, 0);

	PrepareMessageS expect;
	bool ret = expect.ParseFrom(buffer, size);
	ASSERT_TRUE(ret);

	EXPECT_TRUE(strncmp(expect.home_port_.data(), gms.home_port_.data(), expect.home_port_.size() ) == 0);
	EXPECT_TRUE(strncmp(expect.dest_host_port_.data(), gms.dest_host_port_.data(), expect.dest_host_port_.size() ) == 0);
	delete buffer;
}


#undef  TEST_R




