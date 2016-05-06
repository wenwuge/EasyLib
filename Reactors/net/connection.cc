#include "connection.h"


TcpConnection::TcpConnection(EventLoopThread * thread, int fd)
{
}
void TcpConnection::Send(void * data, uint32_t len)
{
}
void TcpConnection::Shutdown()
{
}

void TcpConnection::ForceClose()
{
}
void TcpConnection::ConnectionEstablished() 
{
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
}

void TcpConnection::HandleClose()
{
}

void TcpConnection::HandleError()
{
}
