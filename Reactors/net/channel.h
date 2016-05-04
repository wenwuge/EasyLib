#ifndef __CHANNEL
#define __CHANNEL

#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>
#include <Timestamp.h>
#include <poll.h>

#define  kNoneEvent 0 
#define  kReadEvent (POLLIN|POLLPRI)
#define  kWriteEvent POLLOUT
using namespace muduo;
class Actor;
class Channel{
public:
    typedef boost::function<void()> EventCallback;
    typedef boost::function<void(Timestamp)> ReadEventCallback;
    Channel(Actor * actor, int fd);
    //void handleEvent(Timestamp receiveTime);
    void setReadCallback(const ReadEventCallback& cb)
    { readCallback_ = cb; }
    void setWriteCallback(const EventCallback& cb)
    { writeCallback_ = cb; }
    void setCloseCallback(const EventCallback& cb)
    { closeCallback_ = cb; }
    void setErrorCallback(const EventCallback& cb)
    { errorCallback_ = cb; }
    void HandleEventWithGuard(Timestamp receiveTime);
    void enableReading() { events_ |= kReadEvent; Update(); }
    void disableReading() { events_ &= ~kReadEvent; Update(); }
    void enableWriting() { events_ |= kWriteEvent; Update(); }
    void disableWriting() { events_ &= ~kWriteEvent; Update(); }
    void disableAll() { events_ = kNoneEvent; Update(); }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    void Remove();
    void HandleEvent(Timestamp& timestamp);
    void set_revents(int events){revents_ = events;}
    int events(){return events_;}
    int index(){return index_;}
    void set_index(int index){index_ = index;}
    int fd(){return fd_;}
private:
    void Update();
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
    int index_;

private:
    bool tied_;
    bool eventHandling_;
    boost::weak_ptr<void>  tie_;

    int fd_;
    int events_;
    int revents_;
    Actor* actor_;
};
#endif
