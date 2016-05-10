#ifndef __BUFFER
#define __BUFFER
#include <vector>
#include <iostream>
#include <errno.h>
#include <sys/socket.h>
using namespace std;
class Buffer{
private:
    vector<char> buffer_;
    size_t read_index_;
    size_t write_index_;
public:
    
    static const size_t kInitialSize = 1024;
    Buffer(size_t initialSize = kInitialSize):
        buffer_(initialSize),read_index_(0),
        write_index_(0){
    }
    void Retrieve(size_t len){
        if(len < ReadableBytes()){
            read_index_ += len;
        }else{
            read_index_ = 0;
            write_index_ = 0;
        }
    }
    
    const char* Peek()
    {
        return begin() + read_index_; 
    }

    size_t ReadableBytes() const{
        return write_index_ - read_index_;
    }

    size_t WriteableBytes() const{
        return buffer_.size() - write_index_;
    }

    size_t ReadFromSocket(int fd, int& saved_errno){
        char extrabuf[65536];
        struct iovec vec[2];
        const size_t writable = WriteableBytes();
        vec[0].iov_base = begin()+write_index_;
        vec[0].iov_len = writable;
        vec[1].iov_base = extrabuf;
        vec[1].iov_len = sizeof(extrabuf);
        
        const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;
        const ssize_t n = readv(fd, vec, iovcnt);
        if (n < 0)
        {
            saved_errno = errno;
        }
        else if (static_cast<size_t>(n) <= writable)
        {
            write_index_ += n;
        }
        else
        {
            write_index_ = buffer_.size();
            append(extrabuf, n - writable);
        }
        return n;

    }
    
    void append(const char* /*restrict*/ data, size_t len)
    {
        EnsureWritableBytes(len);
        std::copy(data, data+len, begin() + write_index_);
        write_index_ += len;
    }
private:
    char* begin(){
        return &*buffer_.begin();
    }

    void MakeSpace(int len){
        if((read_index_ + WriteableBytes()) < len){
                buffer_.resize(write_index_  + len);
        }else{
            size_t reads = ReadableBytes();
            copy(begin() + read_index_,
                 begin() + write_index_,
                 begin());

            read_index_ = 0;
            write_index_ = reads;
        }
    }

    //ensure the left space can store the data,
    //otherwise, resize or mv unwritedata
    void EnsureWritableBytes(size_t len)
    {
        if(len >  WriteableBytes()){
            MakeSpace(len);
        }
    }


};

#endif
