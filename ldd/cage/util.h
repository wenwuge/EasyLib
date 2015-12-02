// util.h (2013-09-24)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_CAGE_UTIL_H_
#define LDD_CAGE_UTIL_H_

#include <string>
#include <stdint.h>
#include <sstream>
#include <boost/operators.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/assign/list_of.hpp>

namespace ldd {
namespace cage {

class SequentialNode : boost::less_than_comparable<SequentialNode> {
public:
    SequentialNode(const std::string& prefix);
    bool Parse(const std::string& name);
    void Clear() { return name_.clear(); sequence_ = 0; }
    bool IsValid() const { return !name_.empty(); }

    const std::string& prefix() const { return prefix_; }
    const std::string& name() const { return name_; }
    uint32_t sequence() const { return sequence_; }
    bool operator < (const SequentialNode& rhs) const;
private:
    std::string prefix_;
    std::string name_;
    uint32_t sequence_;
};

template <class SequenceSequenceT>
inline typename boost::range_value<SequenceSequenceT>::type
JoinPath(const SequenceSequenceT& s) {
    typedef typename boost::range_value<SequenceSequenceT>::type ResultT;
    typedef typename boost::range_const_iterator<SequenceSequenceT>::type IterT;
    static const ResultT kSlash = "/";
    static const ResultT kEmpty = "";

    IterT pos = boost::begin(s);
    IterT end = boost::end(s);
    std::vector<ResultT> elements(1, kEmpty);
    for (; pos != end; ++pos) {
        ResultT element = boost::trim_copy_if(*pos, boost::is_any_of(kSlash));
        if (boost::empty(element)) {
            continue;
        }
        elements.push_back(element);
    }
    return boost::join(elements, kSlash);
}

inline std::string JoinPath(const std::string& p1, const std::string& p2) {
    return JoinPath(boost::assign::list_of(p1)(p2));
}

inline std::string JoinPath(const std::string& p1, const std::string& p2,
        const std::string& p3) {
    return JoinPath(boost::assign::list_of(p1)(p2)(p3));
}

inline std::string JoinPath(const std::string& p1, const std::string& p2,
        const std::string& p3, const std::string& p4) {
    return JoinPath(boost::assign::list_of(p1)(p2)(p3)(p4));
}

inline std::string JoinPath(const std::string& p1, const std::string& p2,
        const std::string& p3, const std::string& p4,
        const std::string& p5) {
    return JoinPath(boost::assign::list_of(p1)(p2)(p3)(p4)(p5));
}

inline std::string JoinPath(const std::string& p1, const std::string& p2,
        const std::string& p3, const std::string& p4,
        const std::string& p5, const std::string& p6) {
    return JoinPath(boost::assign::list_of(p1)(p2)(p3)(p4)(p5)(p6));
}

std::string GetLeaf(const std::string& path);
std::string RemoveLeaf(const std::string& path);

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_UTIL_H_
