
#ifndef __LDD_COURIER_REDO_POLICY_H_
#define __LDD_COURIER_REDO_POLICY_H_

#include <stdint.h>
#include <string>

namespace ldd{
namespace courier{

class RedoPolicy{
public:
	RedoPolicy();
	virtual				~RedoPolicy();
	virtual bool		ShouldRety()= 0;
	virtual int32_t		GetRedoTimeInterval() = 0;
	virtual RedoPolicy* Copy() = 0;
	void				incr();
	int32_t				CurTimes();
private:
	RedoPolicy(const RedoPolicy&);
	RedoPolicy& operator=(const RedoPolicy&);
private:
	int cur_retry_times;
};


}//namespace courier
}//namespace ldd




#endif  // LDD_COURIER_RWLOCK_H_





