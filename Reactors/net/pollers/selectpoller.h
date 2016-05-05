#ifndef __SELECTPOLLER
#define __SELECTPOLLER
#include "../poller.h"
#include <Timestamp.h>
#include <sys/select.h>
#include "../channel.h"
class SelectPoller:public Poller{
public:
    SelectPoller();
    ~SelectPoller();
    virtual Timestamp Poll(int timeout_ms, ChannelList& active_channels);
    virtual bool UpdateChannel(Channel* channel);
    virtual bool RemoveChannel(Channel* channel);

private:
    void FillActiveChannels(fd_set& readset, fd_set& writeset, ChannelList& active_channels);

private:
    fd_set readfds_;
    fd_set writefds_;
    list<int> fds_;
    int max_fd_;

};

#endif
