
#ifndef __LDD_COURIER_REDO_POLICY_SIMPLE_H_
#define __LDD_COURIER_REDO_POLICY_SIMPLE_H_

#include <stdint.h>
#include "redo_policy.h"

namespace ldd{
namespace courier{

class RedoPolicySimple : public RedoPolicy{
public:
	RedoPolicySimple(int max_retry_times, int32_t retry_interval_ms);
	virtual		~RedoPolicySimple();

	bool		ShouldRety();
	int32_t		GetRedoTimeInterval();
	virtual		RedoPolicy* Copy();
private:
	int max_retry_times_;
	int32_t retry_interval_ms_;
};



}//namespace courier
}//namespace ldd




#endif  // LDD_COURIER_RWLOCK_H_





