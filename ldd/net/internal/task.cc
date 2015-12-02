// task.cc (2013-10-18)
// Yan Gaofeng (yangaofeng@360.cn)

#include <event2/event.h>
#include <glog/logging.h>

#include <ldd/net/buffer.h>

#include "task.h"

namespace ldd { namespace net {

#ifdef RES_COUNTER
util::Atomic<uint64_t> Task::next_id_;
util::Atomic<uint64_t> Task::free_id_;
#endif

Task::Task()
    : is_set_(false),
      fd_(-1), 
      pos_(0)
{
#ifdef RES_COUNTER
    next_id_++;
#endif
}

Task::~Task()
{
#ifdef RES_COUNTER
    free_id_++;
#endif
}

void Task::Set(const boost::shared_ptr<Buffer> &buffer, int fd) 
{
    CHECK(is_set_ == false);
    CHECK(fd > 0);

    DLOG(INFO) << "set task, buffer bytes: " << buffer->size()
        << ", fd: " << fd;

    buffer_ = buffer;
    fd_ = fd;
    pos_ = 0;
    is_set_ = true;
}

void Task::Reset()
{
    if (!is_set_) {
        return;
    }

    buffer_.reset();
    fd_ = -1;
    pos_ = 0;
    is_set_ = false;
}

int ReadTask::Read()
{
    CHECK(is_set_);

    int remain = buffer_->size() - pos_;
    if (remain <= 0) {
        LOG(WARNING) << "fd: " << fd_
            << ", remain bytes : " << remain
            << ", buffer size: " << buffer_->size()
            << ", pos : " << pos_;
        return ENOBUFS;
    }

    do {
        char *p = const_cast<char *>(buffer_->data());
        p += pos_;
        int bytes = read(fd_, p, remain);
        if (bytes > 0) {
            DLOG(INFO) << "fd: " << fd_
                << ", read bytes: " << bytes;
            pos_ += bytes;
            remain -= bytes;
        }
        else if (bytes == 0) {
            DLOG(ERROR) << "socket is closed when read, fd: " << fd_;
            return ECONNRESET;
        }
        else if (bytes == -1) {
            if (errno == EINTR) {
                continue;
            }
            else if (errno == EAGAIN
                    || errno == EWOULDBLOCK) {
                DLOG(INFO) << "socket would blcok, fd: " << fd_;
                return errno;
            }
            else {
                DLOG(ERROR) << "unexpect error when read socket: " << fd_
                    << ", errno: " << errno << ", error: " << strerror(errno);
                return errno;
            }
        }
    } while (remain > 0);

    if (pos_ == buffer_->size()) {
        DLOG(INFO) << "read task complete, read bytes: " << pos_
            << ", fd: " << fd_;
        return 0;
    }
    else {
        LOG(FATAL) << "unknown error when read socket, fd: " << fd_;
        return -1;
    }
}

int ReadTask::Write()
{
    LOG(FATAL) << "should not call write in read task";
    return -1;
}

int WriteTask::Write()
{
    CHECK_GE(fd_, 0);

    int remain = buffer_->size() - pos_;
    if (remain <= 0) {
        //note: remain = 0,表示只判断可写事件，不是真的写
        LOG(WARNING) << "write bytes is : " << remain
            << ", fd: " << fd_;
        return 0;
    }

    do {
        int bytes = send(fd_, &(buffer_->data()[pos_]), remain, MSG_NOSIGNAL);
        if (bytes > 0) {
            DLOG(INFO) << "write bytes: " << bytes
                << ", fd: " << fd_;
            pos_ += bytes;
            remain -= bytes;
        }
        else if (bytes == 0) {
            DLOG(INFO) << "socket is closed when write";
            return ECONNRESET;
        }
        else if (bytes == -1) {
            if (errno == EINTR) {
                continue;
            }
            else if (errno == EAGAIN
                    || errno == EWOULDBLOCK) {
                DLOG(INFO) << "socket would block when write";
                return errno;
            }
            else if (errno == EPIPE) {
                DLOG(ERROR) << "socket is closed by client";
                return errno;
            }
            else {
                DLOG(ERROR) << "unexpected error when write: " << strerror(errno);
                return errno;
            }
        }
    } while (remain > 0);

    if (pos_ == buffer_->size()) {
        DLOG(INFO) << "write complete, write bytes: " << pos_
            << ", fd: " << fd_;
        return 0;
    }
    else {
        LOG(FATAL) << "unknown error when write socket, fd: " << fd_
            << ", remain: " << remain
            << ", pos: " << pos_
            << ", buffer size: " << buffer_->size();
        return -1;
    }
}

int WriteTask::Read()
{
    LOG(FATAL) << "should not call read in write task";
    return -1;
}

} /*net*/
} /*ldd*/

