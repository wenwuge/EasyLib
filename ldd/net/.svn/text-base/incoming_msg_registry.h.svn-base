// incoming_msg_registry.h (2013-05-29)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_INCOMING_MSG_REGISTRY_H_
#define LDD_NET_INCOMING_MSG_REGISTRY_H_

#include "ldd/net/message.h"
#include "ldd/net/registry.h"
#include "ldd/net/incoming_msg.h"

namespace ldd {
namespace net {

/*
 * private继承意味根据某物实现出，非is-a关系
 *
 * IncomingMsg是基类
 * Type是int16_t，消息类型，与IncomingMsg的派生类相关联
 *
 * */
class IncomingMsgRegistry : private Registry<IncomingMsg, MessageType> {
public:
    IncomingMsgRegistry() : default_(NULL) {}
    virtual ~IncomingMsgRegistry() { delete default_; }

    boost::shared_ptr<IncomingMsg> NewIncomingMsg(MessageType t) { return MakeShared(t); }

    boost::shared_ptr<DynamicIncomingMsg> NewDefaultIncomingMsg() {
        if (default_ != NULL) {
            return default_->MakeShared();
        }
        return boost::shared_ptr<DynamicIncomingMsg>();
    }

    // 调用Registry的Register的方法实现RegisterIncomingMsg,不带参数
    // Msg是IncomingMsg的派生类
    template <class Msg>
    void RegisterIncomingMsg() { Register<Msg>(Msg::kType); }
    template <class Msg, class A1>
    void RegisterIncomingMsg(A1 a1) { Register<Msg>(Msg::kType, a1); }
    template <class Msg, class A1, class A2>
    void RegisterIncomingMsg(A1 a1, A2 a2) { Register<Msg>(Msg::kType, a1, a2); }
    template <class Msg, class A1, class A2, class A3>
    void RegisterIncomingMsg(A1 a1, A2 a2, A3 a3) { Register<Msg>(Msg::kType, a1, a2, a3); }
    template <class Msg, class A1, class A2, class A3, class A4>
    void RegisterIncomingMsg(A1 a1, A2 a2, A3 a3, A4 a4) { Register<Msg>(Msg::kType, a1, a2, a3, a4); }
    template <class Msg, class A1, class A2, class A3, class A4, class A5>
    void RegisterIncomingMsg(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) { Register<Msg>(Msg::kType, a1, a2, a3, a4, a5); }
    template <class Msg, class A1, class A2, class A3, class A4, class A5, class A6>
    void RegisterIncomingMsg(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) { Register<Msg>(Msg::kType, a1, a2, a3, a4, a5, a6); }
    template <class Msg, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    void RegisterIncomingMsg(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) { Register<Msg>(Msg::kType, a1, a2, a3, a4, a5, a6, a7); }
    template <class Msg, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    void RegisterIncomingMsg(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) { Register<Msg>(Msg::kType, a1, a2, a3, a4, a5, a6, a7, a8); }
    template <class Msg, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    void RegisterIncomingMsg(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) { Register<Msg>(Msg::kType, a1, a2, a3, a4, a5, a6, a7, a8, a9); }
    template <class Msg, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    void RegisterIncomingMsg(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) { Register<Msg>(Msg::kType, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); }

    template <class Msg>
    void RegisterDefaultIncomingMsg() {
        assert(!default_);
        default_ = Factory<DynamicIncomingMsg>::NewFactory<Msg>();
    }
    template <class Msg, class A1>
    void RegisterDefaultIncomingMsg(A1 a1) {
        assert(!default_);
        default_ = Factory<DynamicIncomingMsg>::NewFactory<Msg>(a1);
    }
    template <class Msg, class A1, class A2>
    void RegisterDefaultIncomingMsg(A1 a1, A2 a2) {
        assert(!default_);
        default_ = Factory<DynamicIncomingMsg>::NewFactory<Msg>(a1, a2);
    }
    template <class Msg, class A1, class A2, class A3>
    void RegisterDefaultIncomingMsg(A1 a1, A2 a2, A3 a3) {
        assert(!default_);
        default_ = Factory<DynamicIncomingMsg>::NewFactory<Msg>(a1, a2, a3);
    }
    template <class Msg, class A1, class A2, class A3, class A4>
    void RegisterDefaultIncomingMsg(A1 a1, A2 a2, A3 a3, A4 a4) {
        assert(!default_);
        default_ = Factory<DynamicIncomingMsg>::NewFactory<Msg>(a1, a2, a3, a4);
    }
    template <class Msg, class A1, class A2, class A3, class A4, class A5>
    void RegisterDefaultIncomingMsg(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
        assert(!default_);
        default_ = Factory<DynamicIncomingMsg>::NewFactory<Msg>(a1, a2, a3, a4, a5);
    }
    template <class Msg, class A1, class A2, class A3, class A4, class A5, class A6>
    void RegisterDefaultIncomingMsg(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) {
        assert(!default_);
        default_ = Factory<DynamicIncomingMsg>::NewFactory<Msg>(a1, a2, a3, a4, a5, a6);
    }
    template <class Msg, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    void RegisterDefaultIncomingMsg(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) {
        assert(!default_);
        default_ = Factory<DynamicIncomingMsg>::NewFactory<Msg>(a1, a2, a3, a4, a5, a6, a7);
    }
    template <class Msg, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    void RegisterDefaultIncomingMsg(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) {
        assert(!default_);
        default_ = Factory<DynamicIncomingMsg>::NewFactory<Msg>(a1, a2, a3, a4, a5, a6, a7, a8);
    }
    template <class Msg, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    void RegisterDefaultIncomingMsg(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) {
        assert(!default_);
        default_ = Factory<DynamicIncomingMsg>::NewFactory<Msg>(a1, a2, a3, a4, a5, a6, a7, a8, a9);
    }
    template <class Msg, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    void RegisterDefaultIncomingMsg(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) {
        assert(!default_);
        default_ = Factory<DynamicIncomingMsg>::NewFactory<Msg>(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    }
private:
    Factory<DynamicIncomingMsg>* default_;
};

} // namespace net
} // namespace ldd

#endif // LDD_NET_INCOMING_MSG_REGISTRY_H_

