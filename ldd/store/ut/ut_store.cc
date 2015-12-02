
#include <stdint.h>
#include <glog/logging.h>
#include <gtest/gtest.h>	
#include "ut_context.h"
#include "net/channel.h"
#include "net/event_loop.h"
#include "net/client.h"
#include "store_proto.h"
#include <gflags/gflags.h>

#define TEST_R(t, n) void MyTest_##t##_##n() 
using namespace ldd::protocol;
using namespace ldd::net;
using namespace ldd::storage;

boost::shared_ptr<Channel> g_channel;

DEFINE_string(key, "my_key", " key ");
DEFINE_int32(bucket, 0 , "bucket id ");
DEFINE_int32(ns, 1 , "namespace id");
DEFINE_int32(number, 1, "test suit number");
DEFINE_int32(port, 40000, "listen port");
DEFINE_string(host, "0.0.0.0", "server host");


//TEST_R(CONTEXT, ReadContext){
void GetMsg(){
    fprintf(stderr, "GetMsg() \n");
	GetMessageS* mgs = new GetMessageS;
	mgs->u16Ns_      = FLAGS_ns;
	mgs->key_		 =  FLAGS_key;

	mgs->u16BucketID_ = FLAGS_bucket;
	boost::shared_ptr<ReadContext> c(new ReadContext(mgs));
    g_channel->Post(c, ldd::util::TimeDiff::Milliseconds(500), true);
}

//void GetMsg{TEST_R(CONTEXT, CheckContext){
void Check(){
	CheckMessageS* cms = new CheckMessageS;
	cms->u16Ns_		 = 1;
	cms->u16BucketID_ = 2;
	cms->key_		 = "my_key";

    boost::shared_ptr<CheckContext> c(new CheckContext(cms));
	g_channel->Post(c, ldd::util::TimeDiff::Milliseconds(500), true);
}

//TEST(CONTEXT, MutateContext){
void Mutate(){
	MutateMessageS* mms = new MutateMessageS;
	mms->u16Ns_	= FLAGS_ns;
	mms->u16BucketID_ = FLAGS_bucket;
	stMutation & sm = mms->mutation_;
	//sm.key = "MutateContext_key";
	sm.key	= "my_key";
	sm.opt_type = 1;
	sm.has_expected_val = 0;
	sm.expected_value	= "MutateContext_value";
	sm.has_expected_version	= 0;
	sm.expected_version	    = 24;
	//sm.value				= "MutateContext_value";
	sm.value		= "my_value";

    boost::shared_ptr<MutateContext> c(new MutateContext(mms));
	g_channel->Post(c, ldd::util::TimeDiff::Milliseconds(500), true);
}


//TEST_R(CONTEXT, ListContext){
void List(){
	ListMessageS *lms = new ListMessageS;
	lms->u16Ns_		= 1;
	lms->key_		= ""; //"MutateContext_key";
	lms->u32limit_	= 500;
	lms->u8position_ = 1;	

    boost::shared_ptr<ListContext> c(new ListContext(lms));
	g_channel->Post(c, ldd::util::TimeDiff::Milliseconds(500), true);
}

//TEST_R(CONTEXT, AtomicIncrContext){
void Incr(){
	/*MutateMessageS* mms = new MutateMessageS;
	mms->u16Ns_	= 7938;
	mms->u16BucketID_ = 2;
	stMutation & sm = mms->mutation_;
	sm.key = "AtomicIncrMessageS_Key";
	sm.OptType = 1;
	sm.has_expected_val = 0;
	sm.has_expected_version	= 0;
	sm.value				= "1222";

	MutateContext* c = new MutateContext(mms);
	g_channel->Post(c);


	sleep(2);*/

	{AtomicIncrMessageS* ats = new AtomicIncrMessageS;
	ats->u16Ns_		  =	 7938;
	ats->u16BucketID_ = 2;
	ats->key_		  = "AtomicIncrMessageS_Key";
	ats->s32operand_  = 30000;
	ats->s32initial_  = 10890;
	ats->u64ttl_	  = 0;
	
    boost::shared_ptr<AtomicIncrContext> c(new AtomicIncrContext(ats));
	g_channel->Post(c, ldd::util::TimeDiff::Milliseconds(500), true);
}
}


//TEST_R(CONTEXT, AtomicAppendContext){
void Append(){
	AtomicAppendMessageS* aas = new AtomicAppendMessageS;
	aas->key_ = "MutateContext_key";
	aas->u16Ns_	= 7938; aas->u16BucketID_ = 2;
	aas->u8position_ = 1;
	aas->content_ = "append_head_";
	aas->u64ttl_ = 19999999;
	
    boost::shared_ptr<AtomicAppendContext> c(new AtomicAppendContext(aas));
	g_channel->Post(c, ldd::util::TimeDiff::Milliseconds(500), true);
}


#undef  TEST_R


// TEST_R  if   // 10.119.94.91     10.16.28.11
// // ./ut_store --port=40000 --host=10.16.28.11 --number=1 -bucket=0 -ns=1 -logtostderr -key=

int main(int argc, char **argv){
	int ret = 0;
	google::InitGoogleLogging(argv[0]);		// ./your_application --log_dir=.
	google::ParseCommandLineFlags(&argc, &argv, false);

	testing::InitGoogleTest(&argc, argv);
	
	if (argc <  2){
		fprintf(stderr, "usage: ./ut_store number!\n");
		fprintf(stderr, "case 1: GetMsg()\n");
		fprintf(stderr, "case 2: Check()\n");
		fprintf(stderr, "case 3: Mutate()\n");
		fprintf(stderr, "case 4: List()\n");
		fprintf(stderr, "case 5: Incr()\n");
		fprintf(stderr, "case 6: Append()\n");
		return 1;
	}
	


    ldd::net::EventLoop event_loop;
    ldd::net::Client::Options options;
    options.pulse_interval = 200;
    options.connect_timeout = 200;
    options.resolve_timeout = 200;
    //options.notify_connected = NotifyConnected;
    //options.notify_connecting = NotifyConnecting;
    //options.notify_closed = NotifyClosed;

    ldd::net::Client client(options);
    g_channel = client.Create(&event_loop, FLAGS_host, FLAGS_port);

    

	ret = RUN_ALL_TESTS();

	fprintf(stderr, "usage: ./ut_store %d!\n", FLAGS_number);
	switch (FLAGS_number){
		case 1:
			GetMsg();
			break;
		case 2:
			Check();
			break;
		case 3:
			Mutate();
			break;
		case 4:
			List();
			break;
		case 5:
			Incr();
			break;
		case 6:
			Append();
			break;
	}

    fprintf(stderr, "before event_loop() \n");
     event_loop.Run();
     getchar();
	
	fprintf(stderr, "BYE BYE !!!!\n");

	LOG(INFO)<<"BYE BYE !!!!";
	google::ShutdownGoogleLogging();

	return 0;
}
