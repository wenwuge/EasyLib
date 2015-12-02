#ifndef LDD_CAGE_INTERNAL_CALLBACK_WRAPPER_H_
#define LDD_CAGE_INTERNAL_CALLBACK_WRAPPER_H_

#include <set>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <glog/logging.h>
#include "ldd/cage/callback.h"
#include "ldd/cage/result.h"
#include "ldd/cage/op.h"

namespace ldd {
namespace cage {

class CallbackWrapper {
public:
    virtual ~CallbackWrapper() {}

    template <class ResultT>
    class Base;
    template <class ResultT>
    class Impl;
};

template <class ResultT>
class CallbackWrapper::Base : public CallbackWrapper {
public:
    Base(const boost::function<void(const ResultT&)>& callback)
        : callback_(callback) {}
    virtual void ProcessResult(ResultT *result) {
        try {
            callback_(*result);
        } catch (const std::exception &e) {
            LOG(ERROR) << "Caught exception: " << e.what();
        }
        delete this;
    }
protected:
    boost::function<void(const ResultT&)> callback_;
};

template <class ResultT>
class CallbackWrapper::Impl : public CallbackWrapper::Base<ResultT> {
    typedef CallbackWrapper::Base<ResultT> BaseT;
public:
    Impl(const boost::function<void(const ResultT&)>& callback)
        : BaseT(callback) {}
};

template <>
class CallbackWrapper::Impl<MultiResult> 
    : public CallbackWrapper::Base<MultiResult> {
    typedef CallbackWrapper::Base<MultiResult> BaseT;
public:
    Impl(size_t count, 
            const boost::function<void(const MultiResult&)>& callback)
        : BaseT(callback) , count_(count)
        , zresults_(new zoo_op_result_t[count]) {}
    virtual ~Impl() {
        delete [] zresults_;
        BOOST_FOREACH(Op::Result* result, results_) {
            delete result;
        }
    }
    virtual void ProcessResult(MultiResult *result) {
        for (int i = 0; i < static_cast<int>(count_); ++i) {
            int rc = zresults_[i].err;
            results_[i]->Update(rc);
        }
        BaseT::ProcessResult(result);
    }

    const size_t count() const { return count_; }
    zoo_op_result_t* zresults() const { return zresults_; }
    std::vector<Op::Result*>& results() { return results_; }

    const std::vector<Op::Result*>& results() const { return results_; }
private:
    size_t count_;
    zoo_op_result_t* zresults_;
    std::vector<Op::Result*> results_;
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_INTERNAL_CALLBACK_WRAPPER_H_
