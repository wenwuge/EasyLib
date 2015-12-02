
#ifndef __LDD_CLIENT_RAW_REDO_POLICY_SIMPLE_H_
#define __LDD_CLIENT_RAW_REDO_POLICY_SIMPLE_H_

#include <stdint.h>
#include "redo_policy.h"

namespace ldd {
namespace client {
namespace raw {

class RedoPolicySimple : public RedoPolicy{
public:
	RedoPolicySimple(int max_retry_times, int64_t time_out);
	virtual		~RedoPolicySimple();

	bool		ShouldRety(int64_t cur_milliseconds);
	virtual		RedoPolicy* Copy();

private:
	int max_retry_times_;
	int64_t time_out_;
	
};



} // namespace ldd {
} // namespace client {
} // namespace raw {




#endif  // LDD_COURIER_RWLOCK_H_





