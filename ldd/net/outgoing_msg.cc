#include <glog/logging.h>
#include <boost/bind.hpp>
#include "ldd/net/outgoing_msg.h"
#include "internal/outgoing_msg_impl.h"

namespace ldd {
namespace net {

#if 0
std::map<OutgoingMsg::Type,  DynamicOutgoingMsg::NextFree> DynamicOutgoingMsg::stat_;
#endif /*RES_COUNTER*/

OutgoingMsg::OutgoingMsg()
    : impl_(new Impl(this))
{
    DLOG(INFO) << "outgoing msg ctor, new impl";
}

OutgoingMsg::~OutgoingMsg() {
    DLOG(INFO) << "outgoing msg dtor, delete impl";
    delete impl_;
}

OutgoingMsg::Id OutgoingMsg::id() const {
    return impl_->id();
}

const boost::shared_ptr<Channel>& OutgoingMsg::channel() const {
    return impl_->channel();
}

void OutgoingMsg::Cancel() {
    channel()->event_loop()->QueueInLoop(
            boost::bind(&OutgoingMsg::DoCancel, shared_from_this()));
}

void OutgoingMsg::DoCancel() {
    impl_->Cancel();
}

DynamicOutgoingMsg::DynamicOutgoingMsg(Type type)
    : type_(type)
{
#if 0
    Inc(type_);
#endif /*RES_COUNTER*/
}

DynamicOutgoingMsg::~DynamicOutgoingMsg()
{
#if 0
    Dec(type_);
#endif /*RES_COUNTER*/
}

void DynamicOutgoingMsg::set_type(Type type)
{ 
    type_ = type;

#if 0
    Inc(type_);
#endif /*RES_COUNTER*/
}

#if 0
void DynamicOutgoingMsg::Inc(Type type)
{
    STAT_IT pos = stat_.find(type);
    if (pos != stat_.end()) {
        (*(pos->second.next_id_))++;
    } else {
        std::pair<STAT_IT, bool> res = 
            stat_.insert(std::make_pair(type, NextFree() ));

        if (res.second) {
            pos = res.first;
            (*(pos->second.next_id_))++;
        } else {
            LOG(ERROR) << "insert atomic uint64_t failed";
        }
    }
}

void DynamicOutgoingMsg::Dec(Type type)
{
    STAT_IT pos = stat_.find(type);
    if (pos != stat_.end()) {
        (*(pos->second.free_id_))++;
    } else {
        LOG(ERROR) << "not found dynamic msg type: " << type;
    }
}
#endif /*RES_COUNTER*/

} /*namespace net*/
} /*namespace ldd*/

