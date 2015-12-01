// buffer.cc (2013-07-05)
// Li Xinjie (xjason.li@gmail.com)

#include <string.h>
#include <stdlib.h>
#include <glog/logging.h>
#include "buffer.h"
#include "backtrace.h"
#include <ostream>

namespace ldd {
namespace net {

#ifdef RES_COUNTER
util::Atomic<uint64_t> Buffer::next_id_;
util::Atomic<uint64_t> Buffer::free_id_;
#endif

Buffer::Buffer()
    : size_(0)
{
#ifdef RES_COUNTER
    next_id_++;
#endif
}

Buffer::Buffer(size_t size)
    : data_((char*)malloc(size), free),
      size_(size)
{
#ifdef RES_COUNTER
    next_id_++;
#endif
}

Buffer::Buffer(const char* data, size_t size)
    : data_((char*)malloc(size), free),
      size_(size)
{
    memcpy(data_.get(), data, size_);

#ifdef RES_COUNTER
    next_id_++;
#endif
}

Buffer::Buffer(const std::string& s)
    : data_((char*)malloc(s.size()), free),
      size_(s.size())
{
    memcpy(data_.get(), s.data(), size_);

#ifdef RES_COUNTER
    next_id_++;
#endif
}


//拷贝构造函数
Buffer::Buffer(const Buffer &buffer) 
{
    data_ = buffer.data_;
    size_ = buffer.size_;

#ifdef RES_COUNTER
    next_id_++;
#endif
}

//拷贝赋值函数
//Buffer& Buffer::operator=(const Buffer &buffer)
//{
//    data_ = buffer.data_;
//    size_ = buffer.size_;
//
//    return *this;
//}

Buffer::~Buffer()
{
#ifdef RES_COUNTER
    free_id_++;
#endif
}

void Buffer::Shrink(size_t n) {
    CHECK_LE(n, size_);
    if (n < size_) {
        void* p = realloc(data_.get(), n);
		size_ = n;
        CHECK_EQ(p, data_.get());
    }
}

void Buffer::ReSize(size_t n) {
    Shrink(n);
}

void Buffer::Reset() {
    data_.reset();
    size_ = 0;
}

void Buffer::Reset(size_t len) {
    data_.reset((char*)malloc(len), free);
    size_ = len;
}

void Buffer::Reset(const std::string& s) {
    if (!s.empty()) {
        data_.reset((char*)malloc(s.size()));
        size_ = s.size();
        memcpy(data_.get(), s.data(), s.size());
    } else {
        Reset();
    }
}

std::string Buffer::ToString() const {
    return std::string(data_.get(), size_);
}

} //namespace net
} //namespace ldd
