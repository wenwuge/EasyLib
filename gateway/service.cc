// service.cc (2013-09-29)
// Yan Gaofeng (yangaofeng@360.cn)
//
#include <ldd/net/endpoint.h>
#include "service.h"

namespace lcs { namespace gateway {

Service::Service(int protocol) : protocol_(protocol)
{
    cur_ = channels_.begin();
}

Service::~Service()
{
    //std::list<boost::shared_ptr<ldd::net::Channel> >::iterator it;
    //for (it = channels_.begin(); it != channels_.end(); it++) {
    //    (*it)->Close();
    //}
}

boost::shared_ptr<ldd::net::Channel> Service::GetChannel()
{
    boost::shared_ptr<ldd::net::Channel> null_channel;

    if (channels_.empty()) {
        LOG(WARNING) << "no channel to be used when get server, service: " 
            << protocol_;
        return null_channel;
    }

    int size = channels_.size();

    while (size-- > 0) {

        //LOG(INFO) << "size: " << size;
        if (cur_ == channels_.end()) {
            cur_ = channels_.begin();
            if (cur_ == channels_.end()) {
                DLOG(WARNING) 
                    << "no channel to be used when get server, protocol: " 
                    << protocol_;
                return null_channel;
            }
        }

        if ((*cur_)->IsConnected()) {
            return *cur_++;
        } else if ((*cur_)->IsConnecting()) {
            //LOG(WARNING) << "channel is connecting when get server: "
            //    << (*cur_)->name();
            cur_++;
            continue;
        } else {
            LOG(WARNING) << "channel is closed when get channel: "
                << (*cur_)->name()
                << ", protocol: "
                << protocol_;
            cur_++;
            continue;
        }
    }

    LOG(WARNING) << "no valid channel to be used when get channel, protocol: " 
        << protocol_;

    return null_channel;
}

void Service::AddChannel(const boost::shared_ptr<ldd::net::Channel> &channel)
{
    int advance = RandomAdvance();

    std::list<boost::shared_ptr<ldd::net::Channel> >::iterator pos
        = channels_.begin();

    std::advance(pos, advance);
    channels_.insert(pos, channel);

    //打乱
    //ReSort();
}

bool Service::RemoveChannel(const std::string &ip, int port)
{
    int count = 0;
    std::list<boost::shared_ptr<ldd::net::Channel> >::iterator pos;
    while (1) {
        pos = std::find_if(channels_.begin(), channels_.end()
                , boost::bind(&Service::ChannelEqual
                    , this, _1, std::make_pair(ip, port)));

        if (pos == channels_.end()) {
            break;
        }

        DLOG(INFO) << "remove channel: " << (*pos)->name();

        if (pos == cur_) {
            cur_++;
        }

        (*pos)->Close();

        channels_.erase(pos);
        count++;
    }

    if (count == 0) {
        LOG(ERROR) << "not found channel with ip: "
            << ip
            << ", port: "
            << port
            << ", protocol: "
            << protocol_;
        return false;
    }

    DLOG(INFO) << "remove " << count << " channels";

    return true;
}

bool Service::ChannelEqual(const boost::shared_ptr<ldd::net::Channel> channel
        , std::pair<std::string, int> ipport)
{
    if (channel->peer_endpoint().address().ToString() == ipport.first
            && channel->peer_endpoint().port() == ipport.second) {
        //DLOG(INFO) << "channel is equal with "
        //    << ipport.first
        //    << ":"
        //    << ipport.second;
        return true;
    }
    else {

        //DLOG(INFO) << "channel not equal with "
        //<< ipport.first
        //<< ":"
        //<< ipport.second
        //<< " in ChannelEqual compare function";

        return false;
    }
}

int Service::RandomAdvance()
{
    if (channels_.empty()) {
        return 0;
    }

    return rand() % (channels_.size() + 1);
}

} /*gateway*/
} /*lcs*/

