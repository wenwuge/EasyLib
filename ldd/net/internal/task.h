// task.h (2013-10-18)
// Yan Gaofeng (yangaofeng@360.cn)
//
#ifndef __TASK_H__
#define __TASK_H__

#include <boost/function.hpp>

class Buffer;

namespace ldd { namespace net {

class Task {
public:
    Task();
    virtual ~Task();

    void Set(const boost::shared_ptr<Buffer> &buffer, int fd);
    bool IsSet() { return is_set_; }
    void Reset();
    boost::shared_ptr<Buffer> &buffer() { return buffer_; }

    int socket() { return fd_; }

    virtual int Read() = 0;
    virtual int Write() = 0;

protected:
    bool is_set_;
    boost::shared_ptr<Buffer> buffer_;
    int fd_;

    uint32_t pos_;

#ifdef RES_COUNTER
private:
    friend class Stat;
    static util::Atomic<uint64_t> next_id_;
    static util::Atomic<uint64_t> free_id_;
    static const uint64_t next_id() { return next_id_; }
    static const uint64_t free_id() { return free_id_; }
#endif
};

class ReadTask : public Task {
public:
    virtual int Read();
    virtual int Write();
};

class WriteTask : public Task {
public:
    virtual int Read();
    virtual int Write();
};

} /*net*/
} /*ldd*/

#endif /*__TASK_H__*/


