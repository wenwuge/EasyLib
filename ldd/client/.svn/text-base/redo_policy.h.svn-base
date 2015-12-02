
#ifndef __LDD_CLIENT_RAW_REDO_POLICY_H_
#define __LDD_CLIENT_RAW_REDO_POLICY_H_

#include <stdint.h>
#include <string>

namespace ldd {
namespace client {
namespace raw {

class RedoPolicy{
public:
	RedoPolicy();
	virtual				~RedoPolicy();
	virtual bool		ShouldRety(int64_t cur_milliseconds)= 0;
	virtual RedoPolicy* Copy() = 0;

	int64_t				FireTime();
	void				FireTime(int64_t time);	
	int32_t				CurTimes();

protected:
	void				Incr();

private:
	RedoPolicy(const RedoPolicy&);
	RedoPolicy& operator=(const RedoPolicy&);
private:
	int		cur_retry_times;
	int64_t fire_time_;
};	


} // namespace ldd {
} // namespace client {
} // namespace raw {




#endif  // LDD_COURIER_RWLOCK_H_







