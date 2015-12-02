#include <glog/logging.h>

#include "ldd/util/time.h"
#include "../redo_simple_policy.h"


namespace ldd {
namespace client {
namespace raw {


RedoPolicy::RedoPolicy():cur_retry_times(0){
}
RedoPolicy::~RedoPolicy(){

}
void RedoPolicy::Incr(){
	++cur_retry_times;
}
int32_t	RedoPolicy::CurTimes(){
	return cur_retry_times;
}

int64_t	RedoPolicy::FireTime(){
	return fire_time_;
}
void RedoPolicy::FireTime(int64_t time){
	fire_time_ = time;
}


/************************************************************************/
/*implementation of policy                                              */
/************************************************************************/
bool RedoPolicySimple::ShouldRety(int64_t cur_milliseconds) {
	Incr();

	LOG(INFO)<<"cur times="<<CurTimes()<<" max_retry_times="<<max_retry_times_;
	if (CurTimes() > max_retry_times_){
		return false;
	}
	int64_t cur_ticks = util::Time::CurrentMilliseconds();
	if (cur_ticks  >= time_out_ + FireTime()){
		LOG(WARNING)<<" TIMEOUT!!! cur_ticks="<<cur_ticks<<" time_out="<<time_out_<<" FireTime()="<<FireTime();
		return false;
	}
	LOG(INFO)<<" cur_ticks="<<cur_ticks<<" time_out="<<time_out_<<" FireTime()="<<FireTime();
	
	return true;
};

RedoPolicySimple::RedoPolicySimple(int max_retry_times, int64_t time_out)
						: max_retry_times_(max_retry_times),
						  time_out_(time_out){
}

RedoPolicySimple::~RedoPolicySimple(){

}


RedoPolicy* RedoPolicySimple::Copy(){
	RedoPolicySimple *p = new RedoPolicySimple(max_retry_times_, time_out_);
	return p;
};



} // namespace ldd {
} // namespace client {
} // namespace raw {









