// const_buffer.h (2013-11-19)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_UTIL_CONST_BUFFER_H_
#define LDD_UTIL_CONST_BUFFER_H_

#include <ldd/util/mutable_buffer.h>

namespace ldd {
namespace util {

class ConstBuffer {
public:
    ConstBuffer(): data_(""), size_(0) {}
    ConstBuffer(const void* buf, size_t size)
        : data_(static_cast<const char*>(buf)), size_(size) {}
    ConstBuffer(const char* buf, size_t size)
        : data_(buf), size_(size ) {}
    ConstBuffer(const MutableBuffer& b): data_(b.data()), size_(b.size()) {}
    ConstBuffer(const std::string& s): data_(s.data()), size_(s.size()) {}
    ConstBuffer(const char* s): data_(s), size_(strlen(s)) {}

    const char* data() const { return data_; }
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    char operator[](size_t n) const {
        assert(n < size());
        return data_[n];
    }

    void Clear() { data_ = ""; size_ = 0; }
    void RemovePrefix(size_t n) {
        assert(n <= size());
        data_ += n;
        size_ -= n;
    }
    void RemovePostfix(size_t n) {
        assert(n <= size());
        size_ -= n;
    }
    std::string ToString() const { return std::string(data_, size_); }

    bool StartsWith(const ConstBuffer& x) const {
        return ((size_ >= x.size_) &&
                (memcmp(data_, x.data_, x.size_) == 0));
    }
    bool EndsWith(const ConstBuffer& x) const {
        return ((size_ >= x.size_) &&
                (memcmp(data_ + size_ - x.size_, x.data_, x.size_) == 0));
    }

    int Compare(const ConstBuffer& b) const;
private:
    const char* data_;
    size_t size_;
};

inline bool operator==(const ConstBuffer& x, const ConstBuffer& y) {
    return ((x.size() == y.size()) &&
            (memcmp(x.data(), y.data(), x.size()) == 0));
}
inline bool operator!=(const ConstBuffer& x, const ConstBuffer& y) {
    return !(x == y);
}

inline int ConstBuffer::Compare(const ConstBuffer& b) const {
    const int min_len = (size_ < b.size_) ? size_ : b.size_;
    int r = memcmp(data_, b.data_, min_len);
    if (r == 0) {
        if (size_ < b.size_) r = -1;
        else if (size_ > b.size_) r = +1;
    }
    return r;
}

} // namespace util
} // namespace ldd

#endif // LDD_UTIL_CONST_BUFFER_H_
