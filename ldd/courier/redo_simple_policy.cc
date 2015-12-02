

#include "redo_simple_policy.h"

namespace ldd{
namespace courier{


RedoPolicy::RedoPolicy():cur_retry_times(0){
}

RedoPolicy::~RedoPolicy(){

}

void RedoPolicy::incr(){
	++cur_retry_times;
}

int32_t	RedoPolicy::CurTimes(){
	return cur_retry_times;
}


bool RedoPolicySimple::ShouldRety() {
	if (CurTimes() > max_retry_times_){
		return false;
	}
	return true;
};

RedoPolicySimple::RedoPolicySimple(int max_retry_times, int32_t retry_interval_ms): 
		max_retry_times_(max_retry_times),retry_interval_ms_(retry_interval_ms){
}

RedoPolicySimple::~RedoPolicySimple(){

}

int32_t RedoPolicySimple::GetRedoTimeInterval(){
	if (CurTimes() == 0 ){
		return 0;
	}else{
		return retry_interval_ms_;
	}
};

RedoPolicy* RedoPolicySimple::Copy(){
	RedoPolicySimple *p = new RedoPolicySimple(max_retry_times_, retry_interval_ms_);
	return p;
};




}//namespace courier
}//namespace ldd







