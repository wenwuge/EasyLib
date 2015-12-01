#include <glog/logging.h>
#include <boost/bind.hpp>
#include "ldd/net/channel.h"
#include "ldd/net/incoming_msg.h"
#include "internal/incoming_msg_impl.h"

namespace ldd {
namespace net {

#if 0
std::map<IncomingMsg::Type,  DynamicIncomingMsg::NextFree> DynamicIncomingMsg::stat_;
#endif

IncomingMsg::IncomingMsg()
    : impl_(new Impl(this))
{
}

IncomingMsg::~IncomingMsg() {
    delete impl_;
}

IncomingMsg::Id IncomingMsg::id() const {
    return impl_->id();
}

bool IncomingMsg::IsCanceled() const {
    return impl_->IsCanceled();
}

const boost::shared_ptr<Channel>& IncomingMsg::channel() const {
    return impl_->channel();
}

void IncomingMsg::Notify(bool done) {
    channel()->event_loop()->QueueInLoop(
            boost::bind(&IncomingMsg::DoNotify, shared_from_this(), done));
}

void IncomingMsg::DoNotify(bool done) {
    impl_->Notify(done);
}

DynamicIncomingMsg::~DynamicIncomingMsg()
{
#if 0
    Dec(type_);
#endif
}

void DynamicIncomingMsg::set_type(Type type)
{
    type_ = type;

#if 0
    Inc(type_);
#endif
}

#if 0
void DynamicIncomingMsg::Inc(Type type)
{
    STAT_IT pos = stat_.find(type);
    if (pos != stat_.end()) {
        (*(pos->second.next_id_))++;
        //if (type == 1000 || type == 1001) {
        //    LOG(INFO) << "Inc [" << type
        //        << ":" << (*(pos->second.next_id_))
        //        << "]";
        //}
    } else {
        std::pair<STAT_IT, bool> res = 
            stat_.insert(std::make_pair(type, NextFree() ));

        if (res.second) {
            pos = res.first;
            (*(pos->second.next_id_))++;
            //if (type == 1000 || type == 1001) {
            //    LOG(INFO) << "Inc [" << type
            //        << ":" << (*(res.first->second.next_id_))
            //        << "]";
            //}
        } else {
            LOG(ERROR) << "insert atomic uint64_t failed";
        }
    }
}

void DynamicIncomingMsg::Dec(Type type)
{
    STAT_IT pos = stat_.find(type);
    if (pos != stat_.end()) {
        //LOG(INFO) << "Dec [" << type
        //    << ":" << (*(pos->second.free_id_))
        //    << "]";
        (*(pos->second.free_id_))++;
    } else {
        LOG(ERROR) << "not found dynamic msg type: " << type;
    }
}
#endif

} // namespace net
} // namespace ldd

