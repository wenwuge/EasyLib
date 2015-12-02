

#ifndef _LDD_UT_EVENT_LIST_H_
#define _LDD_UT_EVENT_LIST_H_

#include "../EventList.h"
#include "../zkcontext.h"

using namespace ldd::courier;

class KeyComparator2{
public:
	KeyComparator2(){}
	int operator()(const ZkContext* a, const ZkContext* b) const{
		if (a->milliseconds_ < b->milliseconds_){
			return -1;
		}else if(a->milliseconds_ > b->milliseconds_){
			return 1;
		}else{
			return 0;
		}
	}
};

#define TEST_R(t, n) void MyTest_##t##_##n() 
TEST_R(EventListXX, GetPut)
{
	KeyComparator2 comparator_;
	typedef EventList<const ZkContext*, KeyComparator2> EventListEx;
	EventListEx event_list(comparator_);
	ZkContext a , b, c, d;
	a.milliseconds_ = 1000; b.milliseconds_ = 500; c.milliseconds_ = 3000; 
	d.milliseconds_ = 1;
	
	const ZkContext* front = NULL;
	
	EXPECT_TRUE(event_list.Empty());

	event_list.Insert(&a);	
	front = event_list.Front();
	EXPECT_TRUE( front == &a);

	EXPECT_TRUE(event_list.Empty() == false);

	event_list.Insert(&b);
	front = event_list.Front();
	EXPECT_TRUE( front == &b);

	event_list.Insert(&c);
	front = event_list.Front();
	EXPECT_TRUE( front == &b);

	event_list.Insert(&d);
	front = event_list.Front();
	EXPECT_TRUE( front == &d);
	
	EXPECT_TRUE(event_list.Empty() == false);


	event_list.PopFront();
	front = event_list.Front();
	EXPECT_EQ(front->milliseconds_, b.milliseconds_);

	event_list.PopFront();
	front = event_list.Front();
	EXPECT_EQ(front->milliseconds_, a.milliseconds_);

	event_list.PopFront();
	front = event_list.Front();
	EXPECT_EQ(front->milliseconds_, c.milliseconds_);

	event_list.PopFront();
	event_list.PopFront();
	EXPECT_TRUE(event_list.Empty());
	//event_list
}

#undef  TEST_R

#endif  // _LDD_UT_EVENT_LIST_H_


