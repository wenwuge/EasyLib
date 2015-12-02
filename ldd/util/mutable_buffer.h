// mutable_buffer.h (2013-11-19)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_UTIL_MUTABLE_BUFFER_H_
#define LDD_UTIL_MUTABLE_BUFFER_H_

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <string>

namespace ldd {
namespace util {

class MutableBuffer {
public:
    MutableBuffer(): data_((char*)""), size_(0) {}
    MutableBuffer(void* buf, size_t size)
        : data_(static_cast<char*>(buf)), size_(size) {}
    MutableBuffer(char* buf, size_t size)
        : data_(buf), size_(size ) {}

    char* data() const { return data_; }
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    char& operator[](size_t n) const {
        assert(n < size());
        return data_[n];
    }
    void Clear() { data_ = (char*)""; size_ = 0; }
    void RemovePrefix(size_t n) {
        assert(n <= size());
        data_ += n;
        size_ -= n;
    }
    void RemovePostfix(size_t n) {
        assert(n <= size());
        size_ -= n;
    }
private:
    char* data_;
    size_t size_;
};

} // namespace util
} // namespace ldd

#endif // LDD_UTIL_MUTABLE_BUFFER_H_
