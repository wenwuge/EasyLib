// stat.cc (2014-08-01)
// Yan Gaofeng (yangaofeng@360.cn)

#include <glog/logging.h>
#include <ostream>
#include "stat.h"
#include "internal/channel_impl.h"
#include "internal/event_loop_impl.h"
#include "internal/incoming_msg_impl.h"
#include "internal/outgoing_msg_impl.h"
#include "internal/event_impl.h"
#include "payload.h"
#include "buffer.h"
#include "internal/task.h"
#include "incoming_msg.h"

namespace ldd { namespace net {

Stat Stat::instance_;

Stat::Stat()
{
}

#if 0
std::string Stat::dynamic_incoming_msg_stat() const
{
    DynamicIncomingMsg::STAT_IT it = DynamicIncomingMsg::stat_.begin();
    std::ostringstream os;
    for (; it != DynamicIncomingMsg::stat_.end(); it++) {

        uint64_t next_id = *(it->second.next_id_);
        uint64_t free_id = *(it->second.free_id_);

        os << " " << it->first;
        os << "[" << free_id;
        os << ":" << next_id;
        os << ":" << next_id - free_id;
        os << "] ";
    }

    return os.str();
}

std::string Stat::dynamic_outgoing_msg_stat() const
{
    DynamicOutgoingMsg::STAT_IT it = DynamicOutgoingMsg::stat_.begin();
    std::ostringstream os;
    for (; it != DynamicOutgoingMsg::stat_.end(); it++) {

        uint64_t next_id = *(it->second.next_id_);
        uint64_t free_id = *(it->second.free_id_);

        os << " " << it->first;
        os << "[" << free_id;
        os << ":" << next_id;
        os << ":" << next_id - free_id;
        os << "] ";
    }

    return os.str();
}

#endif /*RES_COUNTER*/

std::string Stat::Status() const
{
    std::ostringstream os;

#ifdef RES_COUNTER
    uint64_t next_id = IncomingChannel::next_id();
    uint64_t free_id = IncomingChannel::free_id();
    os << " in_channel[" << free_id
        << ":" << next_id
        << ":" <<  next_id - free_id
        << "] ";

    next_id = Server::Impl::next_id();
    free_id = Server::Impl::free_id();
    os << " in_fd[" << free_id
        << ":" << next_id
        << ":" <<  next_id - free_id
        << "] ";

    next_id = OutgoingChannel::next_id();
    free_id = OutgoingChannel::free_id();
    os << " out_channel[" << free_id
        << ":" << next_id
        << ":" <<  next_id - free_id
        << "] ";

    next_id = Client::Impl::next_id();
    free_id = Client::Impl::free_id();
    os << " out_fd[" << free_id
        << ":" << next_id
        << ":" <<  next_id - free_id
        << "] ";

    next_id = IncomingMsg::Impl::next_id();
    free_id = IncomingMsg::Impl::free_id();
    os << " imsg[" << free_id
        << ":" << next_id
        << ":" <<  next_id - free_id
        << "] ";
#if 0
    os << dynamic_incoming_msg_stat();
#endif /*RES_COUNTER*/

    next_id = OutgoingMsg::Impl::next_id();
    free_id = OutgoingMsg::Impl::free_id();
    os << " omsg[" << free_id
        << ":" << next_id
        << ":" <<  next_id - free_id
        << "] ";

#if 0
    os << dynamic_outgoing_msg_stat();
#endif /*RES_COUNTER*/

    next_id = EventLoop::Impl::next_id();
    free_id = EventLoop::Impl::free_id();
    os << " functor[" << free_id
        << ":" << next_id
        << ":" <<  next_id - free_id
        << "]";

    next_id = FdEvent::Impl::next_id();
    free_id = FdEvent::Impl::free_id();
    os << " fdev[" << free_id
        << ":" << next_id
        << ":" <<  next_id - free_id
        << "] ";

    next_id = TimerEvent::Impl::next_id();
    free_id = TimerEvent::Impl::free_id();
    os << " timerev[" << free_id
        << ":" << next_id
        << ":" <<  next_id - free_id
        << "] ";

    next_id = SignalEvent::Impl::next_id();
    free_id = SignalEvent::Impl::free_id();
    os << " sigev[" << free_id
        << ":" << next_id
        << ":" <<  next_id - free_id
        << "] ";

    next_id = Payload::next_id();
    free_id = Payload::free_id();
    os << " payload[" << free_id
        << ":" << next_id
        << ":" <<  next_id - free_id
        << "] ";

    next_id = Buffer::next_id();
    free_id = Buffer::free_id();
    os << " buffer[" << free_id
        << ":" << next_id
        << ":" <<  next_id - free_id
        << "] ";

    next_id = Task::next_id();
    free_id = Task::free_id();
    os << " task[" << free_id
        << ":" << next_id
        << ":" <<  next_id - free_id
        << "] ";
#endif

    return os.str();
}


} /*namespace net*/
} /*namespace ldd*/

