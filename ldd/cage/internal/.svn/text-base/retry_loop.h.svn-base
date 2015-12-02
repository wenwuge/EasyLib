// retry_loop.h (2013-09-11)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_CAGE_INTERNAL_RETRY_LOOP_H_
#define LDD_CAGE_INTERNAL_RETRY_LOOP_H_

#include <boost/intrusive/list.hpp>

namespace ldd {
namespace cage {

class RetryLoop
    : public boost::intrusive::list_base_hook<
                boost::intrusive::link_mode<boost::intrusive::auto_unlink> >{
public:
    virtual ~RetryLoop() {}
    virtual void Start() = 0;
    virtual void Stop() = 0;
    template <class R>
    class Impl;
};

typedef boost::intrusive::list<RetryLoop,
        boost::intrusive::constant_time_size<false> > RetryList;

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_INTERNAL_RETRY_LOOP_H_
