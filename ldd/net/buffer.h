#ifndef LDD_NET_BUFFER_H_
#define LDD_NET_BUFFER_H_

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <ldd/util/atomic.h>

namespace ldd { namespace net {

class Buffer {
public:
    Buffer();
    explicit Buffer(size_t size);
    explicit Buffer(const std::string& s);
    explicit Buffer(const char* p, size_t l);

    Buffer(const Buffer &buffer);
    //Buffer& operator=(const Buffer &buffer);

    ~Buffer();

    void Reset(size_t size);
    void Reset(const std::string& s);
    void Reset();
    
    bool IsEmpty() const { return size() == 0u; }

    char* data() { return data_.get(); }
    const char* data() const { return data_.get(); }
    size_t size() const { return size_; }

    std::string ToString() const;

    // Shrink the buffer, n Must <= size();
    void Shrink(size_t n);

    // Deprecated methods.
    void ReSize(size_t n);
    char* ptr() { return data_.get(); }
    const char* ptr() const { return data_.get(); }
    size_t len() const { return size_; }

private:
    boost::shared_ptr<char> data_;
    size_t size_;


#ifdef RES_COUNTER
private:
    friend class Stat;
    static util::Atomic<uint64_t> next_id_;
    static util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
#endif /*RES_COUNTER*/
};

typedef std::map<uint8_t, Buffer> BufferMap;

}/*net*/
}/*ldd*/


#endif // LDD_NET_BUFFER_H_

