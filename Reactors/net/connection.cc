#include "connection.h"


TcpConnection::TcpConnection(EventLoopThread * thread, int fd):
        channel_(new Channel(thread->GetActor(), fd)),thread_(thread),
        fd_(fd),state_(CLOSED),
        writeable_(false),readable_(false)
{
    channel_->setReadCallback(boost::bind(&TcpConnection::HandleRead,  this, _1));
    channel_->setWriteCallback(boost::bind(&TcpConnection::HandleWrite, this));
    channel_->setCloseCallback(boost::bind(&TcpConnection::HandleClose, this));
    channel_->setErrorCallback(boost::bind(&TcpConnection::HandleError, this));
}
TcpConnection::~TcpConnection()
{
    cout << "tcp connection destruct" << endl;
}


void TcpConnection::SendInLoop(void * data, uint32_t len)
{

    if(state_ != ESTABLISHED){
        cout << "state is not established ,can't send" <<endl;
        return;
    }

    if(send_buffer_.ReadableBytes() > 0){
        //just add write data
        send_buffer_.append(static_cast<const char*>(data), len);

    }else{
        //send immediately,if no space ,store left data into send_buffer_
        int write_cnt = write(fd_, data, len);
        if(write_cnt >= 0){
            if(write_cnt < len){
                send_buffer_.append(static_cast<const char*>(data) + write_cnt , len -write_cnt);
                //enable channel write
                channel_->enableWriting();
            }else{
                if(writecomplete_callback_)
                    writecomplete_callback_(shared_from_this());
            }
        }else{
            if(errno != EWOULDBLOCK){
                cout << "send data error, errno: " << errno <<endl;
                if(errno == EPIPE || errno == ECONNRESET){
                    cout << "EPIPE OR ECONNRESET happen" << endl;
                }
            }
        }
    }   

}

void TcpConnection::Send(void * data, uint32_t len)
{
    assert(data!=NULL&&len>=0);
    if(thread_->IsInLoop()){
        SendInLoop(data, len);
    }else{
        thread_->QueueInLoop(boost::bind(&TcpConnection::SendInLoop, this, data, len));
    }
}
void TcpConnection::Shutdown()
{
}

void TcpConnection::ForceClose()
{
}
void TcpConnection::ConnectionEstablished() 
{
    //add the connection into local thread loop,mainly read event

    std::cout << "tcp connection establised, call established_callback" << endl;

    established_callback_(shared_from_this());
    channel_->enableReading();
    state_ = ESTABLISHED;
    readable_ = true;
    writeable_ = true;

}
void TcpConnection::HandleRead(Timestamp ts)
{
    int saved_errorno  = 0;
    int cnt = recv_buffer_.ReadFromSocket(fd_, saved_errorno);
    
    if(cnt > 0)
        message_callback_(shared_from_this(), &recv_buffer_, ts );
    else if(cnt == 0){
        HandleClose();
    }else{
        errno = saved_errorno; 
        HandleError();
    }

}
void TcpConnection::HandleWrite()
{
    if(!writeable_){
        cout << "connection fd " << fd_ << " is down, can't write again" << endl;
        return ;
    }
    //send data in send_buffer_
    int cnt = write(fd_, send_buffer_.Peek(), send_buffer_.ReadableBytes());
    if(cnt >=0 ){
        send_buffer_.Retrieve(cnt);
        if(send_buffer_.ReadableBytes() == 0){
            channel_->disableWriting();
            if(writecomplete_callback_)
                writecomplete_callback_(shared_from_this());
        }
    }else{
        std::cout << "send data in handlewrite error, errno: " << errno << endl;
    }
}

void TcpConnection::HandleClose()
{
}

void TcpConnection::HandleError()
{
}
