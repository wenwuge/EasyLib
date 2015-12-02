// classification.h (2013-07-15)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_UTIL_DETAIL_ATOMIC_CLASSIFICATION_H_
#define LDD_UTIL_DETAIL_ATOMIC_CLASSIFICATION_H_

#include <boost/type_traits/is_integral.hpp>

namespace ldd {
namespace util {
namespace detail {

template<typename T, bool IsInt = boost::is_integral<T>::value>
struct Classify {
    typedef void Type;
};

template<typename T>
struct Classify <T, true> { typedef int Type; };

template<typename T>
struct Classify <T*, false> { typedef void* Type; };

template<typename T>
struct StorageSizeOf {
    enum _ {
        size = sizeof(T),
        value = (size == 3 ?
                4 : (size == 5 || size == 6 || size == 7 ?
                    8 : size))
    };
};

} // namespace datail
} // namespace util
} // namespace ldd

#endif // LDD_UTIL_DETAIL_ATOMIC_CLASSIFICATION_H_
