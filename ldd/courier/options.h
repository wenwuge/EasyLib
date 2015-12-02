
#ifndef __LDD_COURIER_OPTIONS_H_
#define __LDD_COURIER_OPTIONS_H_

#include <stdint.h>
#include <string>
#include <boost/function.hpp>

namespace ldd{
namespace courier{

typedef boost::function<void(int err_code)>HandleZkError;

class RedoPolicy;

struct Options{
	Options() :timeout(0), redo_policy(NULL){}
	int timeout;
	RedoPolicy* redo_policy;
    HandleZkError cb;
};



}//namespace courier
}//namespace ldd




#endif  // LDD_COURIER_RWLOCK_H_





