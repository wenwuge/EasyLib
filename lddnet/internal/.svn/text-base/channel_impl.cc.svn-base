// channel_impl.cc (2013-08-19)
// Li Xinjie (xjason.li@gmail.com)

#include <event2/dns.h>
#include <event2/util.h>
#include <event2/event.h>

#include <string.h>
#include <glog/logging.h>
#include <sys/socket.h>

#include <sstream>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include "ldd/net/outgoing_msg.h"
#include "ldd/net/incoming_msg.h"
#include "ldd/util/const_buffer.h"
#include "channel_impl.h"
#include "event_loop_impl.h"
#include "outgoing_msg_impl.h"
#include "incoming_msg_impl.h"
#include "incoming_packet.h"
#include "outgoing_packet.h"
#include "pulse_emitter.h"
#include "pulse_checker.h"

namespace ldd {
namespace net {

using namespace std;
using namespace boost;
using namespace ldd::util;

#ifdef RES_COUNTER
util::Atomic<uint64_t> EventCtx::next_id_;
util::Atomic<uint64_t> EventCtx::free_id_;

util::Atomic<uint64_t> DnsCtx::next_id_;
util::Atomic<uint64_t> DnsCtx::free_id_;


util::Atomic<uint64_t> IncomingChannel::next_id_;
util::Atomic<uint64_t> IncomingChannel::free_id_;

util::Atomic<uint64_t> OutgoingChannel::next_id_;
util::Atomic<uint64_t> OutgoingChannel::free_id_;
#endif

util::Atomic<uint64_t> Channel::Impl::next_id_;

void Channel::Impl::EventNotify(int fd, short what, void *arg)
{
    CHECK_NOTNULL(arg);

    EventCtx *ctx = (EventCtx *)arg;

    if (what & EV_READ) {
        DLOG(INFO) << "read event notify, fd: " << fd;
        ctx->impl_->OnRead();
    }
    else if (what & EV_WRITE) {
        DLOG(INFO) << "write event notify, fd: " << fd;
        ctx->impl_->OnWrite();
    }
    else {
        LOG(FATAL) << "unknown event in EventNotify, fd: " << fd;
        return;
    }
}

static const char* kNotifierNames[] = {
    "CLOSED",
    "CONNECTING",
    "CONNECTED",
};

Channel::Impl::Impl(Owner* owner, EventLoop* event_loop)
    : socket_(-1), 
      event_ctx_(NULL),
      owner_(owner),
      event_loop_(event_loop),
      state_(Channel::kClosed),
      events_(0),
      read_state_(kReadHeader),
      socket_ev_(NULL),
      header_(new Buffer(Header::kByteSize)),
      outgoing_msg_id_(-1),
      id_(next_id_++)
{
    DLOG(INFO) << "channel impl ctor, id: " << id_;
    CHECK_NOTNULL(event_loop_);

    socket_ev_ = event_new(event_loop_->impl_->event_base(),
            socket_, 0, EventNotify, (void*)this);
    CHECK_NOTNULL(socket_ev_);
}

Channel::Impl::~Impl() {
    DLOG(INFO) << "channel impl dtor, id: " << id();


    if (socket_ev_ != NULL) {
        event_free(socket_ev_);
        socket_ev_ = NULL;
    }

    if (event_ctx_ != NULL) {
        delete event_ctx_;
        event_ctx_ = NULL;
    }
}

struct event_base *Channel::Impl::event_base() {
    return event_loop()->impl_->event_base();
}

bool Channel::Impl::FillEndpoints(bool remote) {
    if (socket_ == -1) {
        DLOG(WARNING) << "invalid socket fd: " << socket_;
        return false;
    }

    int rc = 0;
    int port = 0;
    struct sockaddr_in  addr;
    socklen_t addrlen = 0;

    if (remote) {
        memset(&addr, 0, sizeof(addr));
        addrlen = sizeof(addr);
        rc = getpeername(socket_, (struct sockaddr *)&addr, &addrlen);
        if (rc != 0) {
            //out put debug info.
            LOG(ERROR) << "get remote host error: " << strerror(rc);
            return false;
        }

        port = ntohs(addr.sin_port);

        peer_endpoint_.set_address(Address(addr.sin_addr));
        peer_endpoint_.set_port(port);
    }

    memset(&addr, 0, sizeof(addr));
    addrlen = sizeof(addr);
    rc = getsockname(socket_, (struct sockaddr *)&addr, &addrlen);
    if (rc != 0) {
        //out put debug info.
        LOG(ERROR) << "get local host error";
        return false;
    }

    port = ntohs(addr.sin_port);

    self_endpoint_.set_address(Address(addr.sin_addr));
    self_endpoint_.set_port(port);

    return true;
}

void Channel::Impl::Notify(const Notifier& notifier) {
    if (notifier) {
        try {
            notifier(owner_->shared_from_this());
        } catch (const std::exception& e) {
            LOG(FATAL) << "Notify " << kNotifierNames[state()] << " exception: "
                << e.what();
        } catch (...) {
            LOG(FATAL) << "Notify " << kNotifierNames[state()] << " error";
        }
    }
}

void Channel::Impl::Post(const boost::shared_ptr<OutgoingMsg>& omsg,
        const ldd::util::TimeDiff& timeout)
{
    DLOG(INFO) << "enter Post";

    if (state() == Channel::kClosed) {
        DLOG(INFO) << "channel is closed when Post";
        omsg->impl_->OnFailed();
        return;
    }

    int32_t id = NextOutgoingMsgId();
    std::pair<OutgoingMsgMap::iterator, bool> r = 
        omsgs_.insert(std::make_pair(id, omsg));
    if (r.second) {
        DLOG(INFO) << "call outgoing msg Init method with timeout: "
            << timeout.ToMilliseconds();

        omsg->impl_->Init(id, owner(), timeout);
    } else {
        LOG(FATAL) << name() << "duplicated outgoing message id = " << id;
    }
}

void Channel::Impl::Write(OutgoingPacket* packet) {

    DLOG(INFO) << "enter Write";

    CHECK_NOTNULL(packet);
    pending_packets_.push_back(packet);

    DoWrite();

    DLOG(INFO) << "call OnOutgoingPacket in Write";
    OnOutgoingPacket();
}

void Channel::Impl::DoWrite()
{
    DLOG(INFO) << "enter DoWrite";

    if (state() != Channel::kConnected) {
        DLOG(INFO) << "channel is not connect when DoWrite";
        return;
    }

    typedef boost::ptr_deque<OutgoingPacket>::auto_type auto_type;

    size_t bytes = 0;
    std::vector<ConstBuffer> buffers;

    DLOG(INFO) << "pending packets: " << pending_packets_.size();

    while (!pending_packets_.empty()) {

        auto_type packet = pending_packets_.pop_front();
        size_t n = packet->SerializeTo(&buffers);
        if (n == 0) {
            //LOG(INFO) << "Seiralize error, n: " << n;
            continue;
        }

        bytes += n;
        writing_packets_.push_back(packet.release());
    }

    if (bytes == 0) {
        DLOG(INFO) << "there is no data to write when DoWrite";
        return;
    }

    boost::shared_ptr<Buffer> load(new Buffer(bytes));
    char *p = load->data();
    std::vector<ConstBuffer>::iterator it = buffers.begin();

    for (; it != buffers.end(); it++) {
        DLOG(INFO) << "size: " 
            << it->size()
            << ", content: "
            << it->ToString();

        char *src = const_cast<char *>(it->data());
        int size = it->size();
        memcpy(p, src, size);
        p += size;
    }

    writing_buffers_.push_back(load);

    if (state() != Channel::kConnected) {
        DLOG(INFO) << "channel is not connected when wirte";
        return;
    }

    RealWrite();
}

void Channel::Impl::RealWrite()
{
    DLOG(INFO) << "enter RealWrite";

    if (!write_task_.IsSet()) {
        if (writing_buffers_.empty()) {
            DoWrite();
            //LOG(INFO) << "no data to write";
            return;
        }

        boost::shared_ptr<Buffer> obuff = writing_buffers_.front();
        write_task_.Reset();
        write_task_.Set(obuff, socket());
        DLOG(INFO) << "create a write task, task write bytes: " 
            << write_task_.buffer()->size();
    }

    int rc = write_task_.Write();
    if (rc == 0) {
        DLOG(INFO) << "complete a write task in real time, task bytes: "
            << write_task_.buffer()->size();
        if (!writing_buffers_.empty()) {
            writing_buffers_.pop_front();
        }
        if (writing_buffers_.empty()) {
            writing_packets_.clear();
        }
        write_task_.Reset();
        //write sucess，continue write
        DoWrite();
        return;
    }
    else if (rc == EAGAIN || rc == EWOULDBLOCK) {
            DLOG(INFO) << "call AddEvent with EV_WRITE, in RealWrite";
            AddEvent(EV_WRITE);
            DLOG(INFO) << "wait for write event";
            return;
    }
    else {
        LOG(ERROR) << name() << " wirte error: " << strerror(rc);
        OnError();
    }
}

void Channel::Impl::CloseSocket()
{
    DLOG(INFO) << "close socket: " << socket_;
    if (socket_ != -1) {
        read_header_task_.Reset();
        read_body_task_.Reset();
        write_task_.Reset();
        DelEvent(EV_READ | EV_WRITE);
        evutil_closesocket(socket_);
        socket_ = -1;
    }
}

void Channel::Impl::Close() {
    if (state() == Channel::kClosed) {
        return;
    }
    OnClose();
}

void Channel::Impl::ClearIncomingMsg() {
    IncomingMsgMap::iterator iit = imsgs_.begin();
    for (; iit != imsgs_.end();) {
        LOG(INFO) << "canceling incoming msg id = " << iit->first;
        (iit++)->second->impl_->DoCancel();
    }
    imsgs_.clear();

    pending_packets_.clear();
    writing_buffers_.clear();
    writing_packets_.clear();
}

void Channel::Impl::ClearOutgoingMsg() {
    DLOG(INFO) << "ClearOutgoingmsg called, omsgs size: " << omsgs_.size();
    OutgoingMsgMap::iterator oit = omsgs_.begin();
    for (; oit != omsgs_.end();) {
        (oit++)->second->impl_->OnFailed();
    }
    omsgs_.clear();

    pending_packets_.clear();
    writing_buffers_.clear();
    writing_packets_.clear();
}

bool Channel::Impl::AddIncomingMsg(const boost::shared_ptr<IncomingMsg>& msg) {
    CHECK_GE(msg->id(), 0);
    std::pair<IncomingMsgMap::iterator, bool> r =
        imsgs_.insert(std::make_pair(msg->id(), msg));
    return r.second;
}

boost::shared_ptr<IncomingMsg> Channel::Impl::GetIncomingMsg(
        int32_t id, bool remove) {
    IncomingMsgMap::iterator it = imsgs_.find(id);
    if (it == imsgs_.end()) {
        return boost::shared_ptr<IncomingMsg>();
    }
    boost::shared_ptr<IncomingMsg> imsg = it->second;
    if (remove) {
        imsgs_.erase(it);
    }
    return imsg;
}

boost::shared_ptr<OutgoingMsg> Channel::Impl::GetOutgoingMsg(
        int32_t id, bool remove) {
    OutgoingMsgMap::iterator it = omsgs_.find(id);
    if (it == omsgs_.end()) {
        return boost::shared_ptr<OutgoingMsg>();
    }
    boost::shared_ptr<OutgoingMsg> omsg = it->second;
    if (remove) {
        omsgs_.erase(it);
    }
    return omsg;
}

void Channel::Impl::OnIncomingPacket() {
    DLOG(INFO) << "enter OnIncomingPacket";
    pulse_checker_->NotifyIncomingPacket();
    pulse_emitter_->NotifyIncomingPacket();
}

void Channel::Impl::OnOutgoingPacket() {
    DLOG(INFO) << "enter OnOutgoingPacket";
    pulse_emitter_->NotifyOutgoingPacket();
}

void Channel::Impl::OnPulseFailed() {
    LOG(ERROR) << name() << "pulse timeout";
    OnError();
}

void Channel::Impl::SetEvent(short events)
{
    DLOG(INFO) << "enter SetEvent, "
        << "prev events: " << events_
        << ", new events: " << events;

    CHECK_GE(socket_, 0);

    if (events == events_) {
        return;
    }

    if (event_ctx_ == NULL) {
        event_ctx_ = new EventCtx(owner(), this);
        CHECK_NOTNULL(event_ctx_); 
    }

    if (events_ != 0 ) {
        CHECK(event_del(socket_ev_) == 0);
    }

    events_ = events;
    if (events_ == 0) {
        return;
    }

    CHECK(event_assign(socket_ev_
                , owner_->impl_->event_base()
                , socket_
                , events_ | EV_PERSIST
                , EventNotify
                , (void*)event_ctx_) == 0);

    CHECK(event_add(socket_ev_, NULL) == 0);
}

void Channel::Impl::OnRead()
{
    DLOG(INFO) << "OnRead called, fd: " << socket_;

    if (state() != Channel::kConnected) {
        LOG(WARNING) << name() 
            << "socket not connected where read event occured, state: "
            << state();
        return;
    }

    switch (read_state_) {
        case kReadHeader:
            ReadHeader();
            break;
        case kReadBody:
            ReadBody();
            break;
        default:
            LOG(FATAL) << "unknown read state";
            return;
    }
}

void Channel::Impl::ReadHeader()
{
    DLOG(INFO) << "enter ReadHeader";

    int rc = 0;
    if (!read_header_task_.IsSet()) {
        DLOG(INFO) << "create a read header task, task read bytes: " 
            << header_->size();
        read_header_task_.Set(header_, socket());
    }

    DLOG(INFO) << "start to read a header";
    rc = read_header_task_.Read();
    if (rc != 0) {
        if (rc != EAGAIN && rc != EWOULDBLOCK) {
            DLOG(INFO) << name() << "Read header error: " << strerror(rc);
            OnError();
            return;
        }

        DLOG(INFO) << "continue read header";
        return;
    }


    //read a complete header
    DLOG(INFO) << "read header bytes: " 
        << read_header_task_.buffer()->size()
        << ", magic: " << (int)read_header_task_.buffer()->data()[0]
        << ", real header magic: "
        << (int)header_->data()[0];

    read_header_task_.Reset();

    //read header complete
    Header::Parser hdr(header_->data());
    if (!hdr.IsValid()) {
        LOG(ERROR) << name() << "Parse invalid header";
        OnError();
        return;
    }

    DLOG(INFO) << name()
        << "Reading "<< name() << " (id=" << hdr.id()
        << " body_type=" << (int)hdr.body_type()
        << " body_size=" << hdr.body_size()
        << " ext_count=" << (int)hdr.ext_count()
        << " ext_len=" << hdr.ext_len()
        << ")";

    //计算body+extra_header+extra的总长度, 由原来的读三次变为读取一次
    int total = hdr.body_size();
    if (hdr.ext_count() > 0 && hdr.ext_len() > 0) {
        //extra header len
        total += ExtHeader::size(hdr.ext_count());
        //ext body len
        total += hdr.ext_len(); 
    }

    packet_size_ = total;

    if (total == 0) {
        DLOG(INFO) << "no payload, and process control packet";
        ProcessPacket();
        return;
    }

    packet_.reset(new Buffer(total));
    DLOG(INFO) << "malloc packet: " << (uint64_t)packet_->data() << ", size: " << total;

    DLOG(INFO) << "create a read task, task bytes: " << total;
    read_body_task_.Set(packet_, socket());

    read_state_ = kReadBody;

    ReadBody();
}

void Channel::Impl::ReadBody()
{
    DLOG(INFO) << "enter ReadBody";

    if (!read_body_task_.IsSet()) {
        LOG(FATAL) << "uninit read task";
        return;
    }

    DLOG(INFO) << "start to read a body";
    int rc = read_body_task_.Read();
    if (rc != 0) {
        if (rc != EAGAIN && rc != EWOULDBLOCK) {
            LOG(ERROR) << name() << "socket error: " << strerror(rc);
            OnError();
            return;
        }

        DLOG(INFO) << "continue read body";
        return;
    }

    read_body_task_.Reset();
    read_state_ = kReadHeader;

    //read body complete, process packet
    DLOG(INFO) << "start process packet";

    ProcessPacket();
}

void Channel::Impl::ProcessPacket()
{
    DLOG(INFO) << "enter ProcessPacket";

    boost::shared_ptr<IncomingPacket> packet;
    switch (header().type()) {
    case Header::kPing:
        DLOG(INFO) << "packet: ping";
        packet = make_shared<IncomingPing>(this);
        break;
    case Header::kPong:
        DLOG(INFO) << "packet: pong";
        packet = make_shared<IncomingPong>(this);
        break;
    case Header::kRequest:
        DLOG(INFO) << "packet: request";
        packet = make_shared<IncomingRequest>(this
                , packet_);
        packet_.reset();
        break;
    case Header::kResponse:
    case Header::kLast:
        DLOG(INFO) << "packet: response or last";
        packet = make_shared<IncomingResponse>(this
                , packet_);
        packet_.reset();
        break;
    case Header::kCancel:
        DLOG(INFO) << "packet: cancel";
        packet = make_shared<IncomingCancel>(this);
        break;
    case Header::kEnd:
        DLOG(INFO) << "packet: end";
        packet = make_shared<IncomingEnd>(this);
        break;
    default:
        LOG(ERROR) << name()
            << "Parsed invalid header type = " << (int)header().type();
        OnError();
        return;
    }

    packet->Process();
}

void Channel::Impl::OnWrite()
{
    DLOG(INFO) << "enter OnWrite, fd: " << socket_;

    //note
    if (state() == Channel::kClosed) {
        LOG(WARNING) << name() << "receive write event when socket is closed";
        return;
    } else if (state() == Channel::kConnecting) {

        DelEvent(EV_WRITE);

        DLOG(INFO) << "receive connect event";
        int len = 0;
        int err = 0;
        len = sizeof(err);
        if (getsockopt(socket_, SOL_SOCKET, SO_ERROR, &err, (socklen_t *)&len)
                < 0) {
            LOG(ERROR) << name() << " get SO_ERROR error:" << strerror(errno);
            err = errno;
        }

        OnConnect(err);

    } else {

        DelEvent(EV_WRITE);

        RealWrite();
    }
}

void Channel::Impl::set_socket(int socket)
{
    DLOG(INFO) << "enter set_socket, fd: " << socket;
    evutil_make_socket_nonblocking(socket);
    socket_ = socket;
}

// IncomingChannel
IncomingChannel::IncomingChannel(Owner* owner, EventLoop* event_loop,
        Server::Impl* server)
    : Super(owner, event_loop),
      server_(server)
{
    DLOG(INFO) << "IncomingChannel ctor called";

#ifdef RES_COUNTER
    next_id_++;
#endif

    InitPulse();
}

std::string IncomingChannel::name() const {
    std::stringstream ss;
    ss << '[' << id() << '@'
        << self_endpoint().ToString()
        << ((type() == Channel::kIncoming) ? " <- " : " -> ")
        << peer_endpoint().ToString()
        << "] ";
    return ss.str();
}

void IncomingChannel::InitPulse()
{
    DLOG(INFO) << "enter InitPulse";

    PulseChecker::Reporter reporter =
        boost::bind(&IncomingChannel::OnPulseFailed, this);
    if (options().pulse_relaxed_checking) {
        DLOG(INFO) << "create RelaxedPulseChecker in InitPulse";
        pulse_checker_.reset(new RelaxedPulseChecker(event_loop(),
                    reporter, util::TimeDiff::Milliseconds(
                        options().pulse_interval)));
    } else {
        DLOG(INFO) << "create StrictPulseChecker in InitPulse";
        pulse_checker_.reset(new StrictPulseChecker(event_loop(),
                    reporter, util::TimeDiff::Milliseconds(
                        options().pulse_interval)));
    }

    PulseEmitter::Pulser pulser = 
        boost::bind(&IncomingChannel::DoPulse, this);
    if (options().pulse_lazy_emitting) {
        DLOG(INFO) << "create LazyPulseEmitter in InitPulse";
        pulse_emitter_.reset(new LazyPulseEmitter(event_loop(),
                    pulser, util::TimeDiff::Milliseconds(
                        options().pulse_interval)));
    } else {
        DLOG(INFO) << "create ReplyPulseEmitter in InitPulse";
        pulse_emitter_.reset(new ReplyPulseEmitter(event_loop(),
                    pulser, util::TimeDiff::Milliseconds(
                        options().pulse_interval)));
    }
}

IncomingChannel::~IncomingChannel() {
    DLOG(INFO) << "IncomingChannel dtor called";
#ifdef RES_COUNTER
    free_id_++;
#endif
}

void IncomingChannel::Open() {
    if (!FillEndpoints(true)) {
        return;
    }

    LOG(INFO) << name() << "opening";
    set_state(Channel::kConnected);
    AddEvent(EV_READ);
    pulse_checker_->Start();
    pulse_emitter_->Start();
    Notify(options().notify_connected);
}


void IncomingChannel::OnPing() {
    DLOG(INFO) << "Ping recved";
    pulse_checker_->NotifyIncomingPulse();
    pulse_emitter_->NotifyIncomingPulse();
}

void IncomingChannel::OnPong() {
    DLOG(ERROR) << name() << "recved pong message";
    OnError();
}

void IncomingChannel::OnError() {
    DLOG(INFO) << "IncomingChannel::OnError called";
    if (state() == Channel::kClosed) {
        return;
    }
    OnClose();
}

void IncomingChannel::OnClose() {
    LOG(INFO) << name() << "closed";
    CHECK_NE(state(), Channel::kClosed);

    set_state(Channel::kClosed);

    //生存期控制，不可以删除
    boost::shared_ptr<Channel> c = owner();

    ClearIncomingMsg();
    ClearOutgoingMsg();

    pulse_checker_->Stop();
    pulse_emitter_->Stop();

    if (socket_ != -1) {
        //LOG(ERROR) << "incoming channel fd: " << socket_;

#ifdef RES_COUNTER
        Server::Impl::free_id_++;
#endif

        CloseSocket();
    }

    Notify(options().notify_closed);

    DLOG(INFO) << "incoming channel use count: " << c.use_count();

    if (event_ctx_ != NULL) {
        delete  event_ctx_;
        event_ctx_ = NULL;
    }
}

void IncomingChannel::DoPulse() {
    DLOG(INFO) << "Pong is sending";
    OutgoingPong* pong = new OutgoingPong(NextOutgoingMsgId());
    Write(pong);
}

// OutgoingChannel
OutgoingChannel::OutgoingChannel(Owner* owner, EventLoop* event_loop,
        Client::Impl* client, const std::string& host, uint16_t port)
    : Super(owner, event_loop),
      client_(client),
      timer_(event_loop),
      host_(host),
      port_(port),
      need_resolve_(false),
      connect_failure_(0),
      connect_delay_(false),
      dns_req_(NULL), 
      dns_ctx_(NULL)
{

#ifdef RES_COUNTER
    next_id_++;
#endif

    Address addr = Address::FromString(host);
    if (!addr.ToU32()) {
        //query_.reset(new tcp::resolver::query(host, ""));
        need_resolve_ = true;
    } else {
        need_resolve_ = false;
        peer_endpoint_.set_address(addr);
    }
    peer_endpoint_.set_port(port);
    InitPulse();
}

OutgoingChannel::OutgoingChannel(Owner* owner, EventLoop* event_loop,
        Client::Impl* client, const Endpoint& remote)
    : Super(owner, event_loop),
      client_(client),
      timer_(event_loop),
      dns_req_(NULL), 
      dns_ctx_(NULL)
{
#ifdef RES_COUNTER
    next_id_++;
#endif

    peer_endpoint_ = remote;
    InitPulse();
}

void OutgoingChannel::InitPulse() {
    PulseChecker::Reporter reporter =
        boost::bind(&OutgoingChannel::OnPulseFailed, this);
    if (options().pulse_relaxed_checking) {
        pulse_checker_.reset(new RelaxedPulseChecker(event_loop(),
                    reporter, util::TimeDiff::Milliseconds(
                        options().pulse_interval)));
    } else {
        pulse_checker_.reset(new StrictPulseChecker(event_loop(),
                    reporter, util::TimeDiff::Milliseconds(
                        options().pulse_interval)));
    }

    PulseEmitter::Pulser pulser = 
        boost::bind(&OutgoingChannel::DoPulse, this);
    if (options().pulse_lazy_emitting) {
        pulse_emitter_.reset(new LazyPulseEmitter(event_loop(),
                    pulser, util::TimeDiff::Milliseconds(
                        options().pulse_interval)));
    } else {
        pulse_emitter_.reset(new ClockPulseEmitter(event_loop(),
                    pulser, util::TimeDiff::Milliseconds(
                        options().pulse_interval)));
    }
}

OutgoingChannel::~OutgoingChannel() {

#ifdef RES_COUNTER
    free_id_++;
#endif

}

void OutgoingChannel::AsyncConnect(const std::string &host
        , uint16_t port
        , int timeout)
{
    DLOG(INFO) << "AsyncConnect called";

    CHECK_EQ(state(), Channel::kConnecting);

    int rc = 0;
	int reuseaddr = 1;
	int keepalive = 1;
	struct sockaddr_in addr; 
	//int sendbuf = 300 * 1024;
	//int recvbuf = 300 * 1024;
	
    socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_ == -1) {
        DLOG(ERROR) << "socket error: " << strerror(errno);
        OnConnect(errno); 
        return;
	}

    rc = evutil_make_socket_nonblocking(socket_);
    if (rc == -1) {
        LOG(ERROR) << "set socket nonblock error: " << strerror(errno);
        OnConnect(errno);
        return;
    }

	setsockopt(socket_
            , SOL_SOCKET
            , SO_REUSEADDR
            , (char *)&reuseaddr
            , sizeof(reuseaddr));

	setsockopt(socket_
            , SOL_SOCKET
            , SO_KEEPALIVE
            , (char *)&keepalive
            , sizeof(keepalive));

	//setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&sendbuf, sizeof(sendbuf));
	//setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&recvbuf, sizeof(recvbuf));
	//setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO
    //, "\240\214\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16);
	//setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO
    //, "\240\214\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16);

	addr.sin_family = AF_INET;
    addr.sin_addr = peer_endpoint().address().ToV4();
	addr.sin_port = htons(port);

	rc = connect(socket_, (struct sockaddr *)&addr, sizeof(struct sockaddr));
	if (rc != 0) {
		int err = errno;
		if (err != EINPROGRESS && err != EWOULDBLOCK) {
            LOG(ERROR) << "connect error: " << strerror(err);
            CloseSocket();
            OnConnect(err);
			return;
		}
	}

#ifdef RES_COUNTER
    Client::Impl::next_id_++;
#endif

    DLOG(INFO) << "create a write task, task size: 0, for connect event";
    boost::shared_ptr<Buffer> conn_buff(new Buffer());
    write_task_.Reset();
    write_task_.Set(conn_buff, socket());
    DLOG(INFO) << "call AddEvent with EV_WRITE, in AsyncConnect";
    AddEvent(EV_WRITE);

    timer_.AsyncWait(
            boost::bind(&OutgoingChannel::OnConnectTimeout, this, owner()),
            util::TimeDiff::Milliseconds(timeout));
}

void OutgoingChannel::Open() {
    set_state(Channel::kConnecting);
    DoConnect();
}

void OutgoingChannel::DoConnect() {
    if (need_resolve_) {
        Resolve();
    } else {
        Connect();
    }
}

void OutgoingChannel::Reconnect() {

    DLOG(INFO) << "enter Reconnect, state: " << state();

    CHECK_EQ(state(), Channel::kConnecting);

    if (connect_failure_ >= options().connect_delay_count) {
        DelayConnect();
    } else {
        DoConnect();
    }
}

void OutgoingChannel::DelayConnect() {
    CHECK_GE(connect_failure_, options().connect_delay_count);

    int n = options().connect_delay_time_initial +
        options().connect_delay_time_step * 
        (connect_failure_ - options().connect_delay_count);
    n = std::min(n, options().connect_delay_time_final);
    util::TimeDiff timeout = util::TimeDiff::Milliseconds(n);
    LOG(INFO) << "Delay connect for " << n << " ms";

    DLOG(INFO) << "OutgoingChannel::DelayConnect call AsyncWait, obj: " << &timer_;
    timer_.AsyncWait(
            boost::bind(&OutgoingChannel::DoDelayConnect, this, owner()),
            timeout);


    connect_delay_ = true;
}

void OutgoingChannel::DoDelayConnect(const boost::shared_ptr<Channel>&) {
    connect_delay_ = false;
    DoConnect();
}



void OutgoingChannel::Resolve() {

    struct evdns_base *dnsbase = evdns_base_new(event_base(), 1);
    CHECK_NOTNULL(dnsbase);

    //note: do'nt forget delete dns_ctx
    dns_ctx_ = new DnsCtx(this, owner(), dnsbase);
    CHECK_NOTNULL(dns_ctx_);

    struct evutil_addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = EVUTIL_AI_CANONNAME;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    DLOG(INFO) << "call evnds_getaddrinfo, ptr: " << this;
    dns_req_ = evdns_getaddrinfo(dnsbase
            , host_.c_str()
            , NULL /* no service name given */
            , &hints
            , OnResolve
            , this);

    if (dns_req_ == NULL) {
        LOG(INFO) << "dns request for " << host_ << " returned immediately";
        return;
    }

    timer_.AsyncWait(
            boost::bind(&OutgoingChannel::OnResolveTimeout, this, owner()),
            util::TimeDiff::Milliseconds(options().resolve_timeout));
}

void OutgoingChannel::OnResolveTimeout(const boost::shared_ptr<Channel>& o) {
    DLOG(INFO) << "OnResolveTimeout called";

    if (dns_req_ != NULL) {
        evdns_getaddrinfo_cancel(dns_req_);
        dns_req_ = NULL;
    }

    connect_failure_++;

    LOG(ERROR) << name() << "DNS resolve timeout";
}


void OutgoingChannel::Connect() {
    DLOG(INFO) << name() << "Connecting...";

    AsyncConnect(peer_endpoint_.address().ToString()
            , peer_endpoint_.port()
            ,  options().connect_timeout);
}

void OutgoingChannel::OnConnectTimeout(const boost::shared_ptr<Channel>& o) {
    LOG(INFO) << name() << " Connect timeout";
    connect_failure_++;
    OnError();
}

void OutgoingChannel::OnConnect(int error)
{
    DLOG(INFO) << "enter OnConnect";


    write_task_.Reset();

    if (state() == Channel::kClosed) {
        timer_.Cancel();
        LOG(WARNING) << name() << "Connect canceled because channel is closed";
        return;
    } 

    if (error != 0 || !FillEndpoints(false)) {
        connect_failure_++;
        timer_.Cancel();
        LOG_IF(ERROR, error) << name() << "Connect failed: " << strerror(error);
        OnError();
        return;
    }

    connect_failure_ = 0;
    LOG(INFO) << name() << "Connected";
    timer_.Cancel();

    set_state(Channel::kConnected);

    pulse_checker_->Start();
    pulse_emitter_->Start();
    Notify(options().notify_connected);

    //enable read event
    DLOG(INFO) << "call AddEvent with EV_READ, in OnConnect, fd: " << socket();
    AddEvent(EV_READ);

    RealWrite();
}

void OutgoingChannel::OnPing() {
    LOG(ERROR) << name() << "recved ping message";
    OnError();
}

void OutgoingChannel::OnPong() {
    pulse_checker_->NotifyIncomingPulse();
    pulse_emitter_->NotifyIncomingPulse();
}

void OutgoingChannel::OnError() {

    DLOG(INFO) << "enter OnError, state: " << state();

    CHECK_NE(state(), Channel::kClosed);

    Channel::State old_state = state();
    set_state(Channel::kConnecting);

    if (old_state == Channel::kConnecting) {
        ClearOutgoingMsg();
        if (socket_ != -1) {
#ifdef RES_COUNTER
            Client::Impl::free_id_++;
#endif
            CloseSocket();
        }
        timer_.Cancel();
    } else if (old_state == Channel::kConnected) {
        ClearIncomingMsg();
        ClearOutgoingMsg();
        pulse_checker_->Stop();
        pulse_emitter_->Stop();
        if (socket_ != -1) {

#ifdef RES_COUNTER
            Client::Impl::free_id_++;
#endif
            CloseSocket();
        }
        if (old_state != state()) {
            Notify(options().notify_connecting);
        }
    } else {
        LOG(FATAL) << name() << "Invalid old state = " << old_state;
    }

    DLOG(INFO) << "state: " << state();

    Reconnect();
}

void OutgoingChannel::OnClose() {
    LOG(INFO) << name() << " closed";
    CHECK_NE(state(), Channel::kClosed);

    Channel::State old_state = state();
    set_state(Channel::kClosed);

    if (old_state == Channel::kConnecting) {

        DLOG(INFO) << "OnClose, channel in connecting";
        ClearOutgoingMsg();

        if (socket_ != -1) {
#ifdef RES_COUNTER
            Client::Impl::free_id_++;
#endif
            CloseSocket();
        }
        timer_.Cancel();

    } else if (old_state == Channel::kConnected) {
        DLOG(INFO) << "OnClose, channel is closed";

        ClearIncomingMsg();
        ClearOutgoingMsg();
        pulse_checker_->Stop();
        pulse_emitter_->Stop();

        if (socket_ != -1) {

#ifdef RES_COUNTER
            Client::Impl::free_id_++;
#endif
            CloseSocket();
        }

    } else {
        LOG(FATAL) << name() << "Invalid old state = " << old_state;
    }

    Notify(options().notify_closed);

    if (event_ctx_ != NULL) {
        delete event_ctx_;
        event_ctx_ = NULL;
    }
}

void OutgoingChannel::DoPulse() {
    DLOG(INFO) << "Ping is sending";
    OutgoingPing* ping = new OutgoingPing(NextOutgoingMsgId());
    Write(ping);
}

std::string OutgoingChannel::name() const {
    std::stringstream ss;
    ss << '[' << id() << "@"
        << self_endpoint().ToString()
        << ((type() == Channel::kIncoming) ? " <- " : " -> ")
        << peer_endpoint().ToString()
        << "] ";
    return ss.str();
}

void OutgoingChannel::OnResolve(int errcode
        , struct evutil_addrinfo *addr
        , void *ptr)
{
    DLOG(INFO) << "OutgoingChannel::OnResolve called, ptr: " << ptr;

    CHECK_NOTNULL(ptr);
    OutgoingChannel *oc = (OutgoingChannel *)ptr;
    CHECK_NOTNULL(oc->dns_ctx_);

    DnsCtx *dns = oc->dns_ctx_;

    //note: live time control, for oc->Error()
    boost::shared_ptr<ldd::net::Channel> channel = oc->owner(); 

    if (errcode != 0) {
        if (errcode != EVUTIL_EAI_CANCEL) {
            oc->timer_.Cancel();
            LOG(ERROR) << "dns resolve failed, "
                << ", error code: " << errcode
                << ", error msg: " << evutil_gai_strerror(errcode);
        } else {
            LOG(WARNING) << "dns resolve cancel, may be timeout";
        }


        DLOG(INFO) << "delete dns ctx";
        evdns_base_free(dns->dns_base_, 0);
        oc->dns_ctx_ = NULL;
        delete dns;

        oc->OnError();

        return;
    }

    if (addr == NULL) {
        LOG(ERROR) << "dns resolve error, addr can not be null";

        DLOG(INFO) << "delete dns ctx";
        evdns_base_free(dns->dns_base_, 0);
        oc->dns_ctx_ = NULL;
        delete dns;

        oc->OnError();

        return;
    }

    oc->DoResolve(errcode, addr);
    evutil_freeaddrinfo(addr);

    DLOG(INFO) << "delete dns ctx";
    evdns_base_free(dns->dns_base_, 0);
    oc->dns_ctx_ = NULL;
    delete dns;

#if 0
    DLOG(INFO) << "OutgoingChannel::OnResolve called, ptr: " << ptr;

    CHECK_NOTNULL(ptr);
    OutgoingChannel *oc = (OutgoingChannel *)ptr;
    CHECK_NOTNULL(oc->dns_ctx_);

    do {
        if (errcode != 0) {
            if (errcode != EVUTIL_EAI_CANCEL) {
                oc->timer_.Cancel();
                LOG(ERROR) << "dns resolve failed, "
                    << ", error code: " << errcode
                    << ", error msg: " << evutil_gai_strerror(errcode);
            } else {
                LOG(WARNING) << "dns resolve cancel, may be timeout";
            }

            break;
        }

        if (addr == NULL) {
            LOG(ERROR) << "dns resolve error, addr can not be null";
            break;
        }

        oc->DoResolve(errcode, addr);

        evutil_freeaddrinfo(addr);

    } while (0);

    boost::shared_ptr<ldd::net::Channel> channel;

    if (oc->dns_ctx_ != NULL) {
        DLOG(INFO) << "delete dns ctx";
        channel = oc->owner(); //note: live time control, for oc->Error()
        evdns_base_free(oc->dns_ctx_->dns_base_, 0);
        delete oc->dns_ctx_;
        oc->dns_ctx_ = NULL;
    }

    oc->OnError();
#endif
}

void OutgoingChannel::DoResolve(int errcode, struct evutil_addrinfo *addr)
{
    if (state() == Channel::kClosed) {
        timer_.Cancel();
        LOG(INFO) << "channel is closed when resolve host: "
            << host_;
    } else if (errcode) {
        LOG(ERROR) 
            << peer_endpoint().ToString()
            << evutil_gai_strerror(errcode);
        connect_failure_++;
        timer_.Cancel();
        OnError();
    } else {
        struct evutil_addrinfo *ai;
        if (addr->ai_canonname) {
            LOG(INFO) << "resolve canon namne: " << addr->ai_canonname;
        }

        ai = addr;
        if (ai != NULL) {
            char buf[128];
            const char *s = NULL;
            if (ai->ai_family == AF_INET) {
                struct sockaddr_in *sin = (struct sockaddr_in *)ai->ai_addr;
                s = evutil_inet_ntop(AF_INET, &sin->sin_addr, buf, 128);
            }
            else if (ai->ai_family == AF_INET6) {
                struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)ai->ai_addr;
                s = evutil_inet_ntop(AF_INET6, &sin6->sin6_addr, buf, 128);
            }
            else {
                LOG(FATAL) << "invalid tcp family";
                return;
            }

            if (s) {
                //LOG(INFO) << s;
                //set ip address
                Address addr = Address::FromString(s);
                if (!addr.ToU32()) {
                    LOG(FATAL) << "invalid resolved ip: " << s;
                    return;
                }

                peer_endpoint_.set_address(addr);

                LOG(INFO) << "Resolved " << host_ << " = "
                    << peer_endpoint_.ToString();

                need_resolve_ = false;
            }
            else {
                LOG(FATAL) << "invalid resolve result, host: "
                    << host_;
                return;
            }
        }
    }

    need_resolve_ = false;
    Connect();
}

} // namespace net
} // namespace ldd
