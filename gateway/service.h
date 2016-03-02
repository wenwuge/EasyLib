// service.h (2013-08-27)
// Yan Gaofeng (yangaofeng@360.cn)

#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <boost/bind.hpp>
#include <glog/logging.h>
#include <boost/shared_ptr.hpp>
#include <list>

#include <ldd/net/channel.h>

namespace lcs { namespace gateway {

class Service {
public:
    explicit Service(int protocol);
    ~Service();

    boost::shared_ptr<ldd::net::Channel> GetChannel();
    size_t size() { return channels_.size(); }
    void AddChannel(const boost::shared_ptr<ldd::net::Channel> &channel);
    bool RemoveChannel(const std::string &ip, int port);

private:
    bool ChannelEqual(const boost::shared_ptr<ldd::net::Channel> channel
            , std::pair<std::string, int> ipport);
    int RandomAdvance();

private:
    std::list<boost::shared_ptr<ldd::net::Channel> > channels_;

    //为了实现轮巡
    std::list<boost::shared_ptr<ldd::net::Channel> >::iterator cur_;

    int protocol_;
};

} /*gateway*/
} /*lcs*/


#endif /*__SERVICE_H__*/

