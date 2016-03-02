// ship.cc (2013-09-13)
// Yan Gaofeng (yangaofeng@360.cn)

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <algorithm>
#include <map>
#include <ldd/net/client.h>
#include <ldd/net/event_loop.h>

#include "ship.h"
#include "gateway_module.h"
#include "session_msg.h"
#include "proto/lcs_session.pb.h"
#include "osl/include/md5.h"
#include "osl/include/crc32.h"

namespace lcs { namespace gateway {

Ship::Ship(GateWayModule *gateway, ldd::net::EventLoop *event_loop) 
    : event_loop_(event_loop)
      , time_cache_(time(NULL))
      , id_(0)
      , gateway_(gateway)
      , user_count_(0)
{
    ldd::net::Client::Options option;
    option.pulse_interval = gateway->server_option().pulse_interval;
    option.pulse_relaxed_checking = gateway->server_option().pulse_relaxed_checking;
    option.pulse_lazy_emitting = gateway->server_option().pulse_lazy_emitting;
    option.notify_connected = boost::bind(&Ship::ServerConnected, this, _1);
    option.notify_closed = boost::bind(&Ship::ServerClosed, this, _1);

    //创建client
    client_ = boost::make_shared<ldd::net::Client>(option);
    if (!client_) {
        LOG(FATAL) << "create client object failed";
        return;
    }

    client_->RegisterDefaultIncomingMsg<PushMsg>(gateway, this);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand((unsigned int)tv.tv_usec);

    StartTimeCacheTimer();
    StartSessionTimer();
}


Ship::~Ship()
{
    users_.clear();
    services_.clear();

    DLOG(INFO) << "Ship dtor called";
}

boost::shared_ptr<User> Ship::GetUser(uint32_t ip, uint16_t port)
{

    LOG(INFO) << "user count :" << users_.size();
    UserIt user_pos = users_.find(ip);
    if (user_pos == users_.end()) {
        //struct in_addr in = {};
        //in.s_addr = htonl(ip);
        //std::string ip_str(inet_ntoa(in));
        //LOG(WARNING) << "not found user by ip, user: " << ip_str
        //    << ":" << port;
        boost::shared_ptr<User> null_client;
        return null_client;
    }

    //if (gateway_->test_option().enable) {
    //    //for push msg test
    //    if (!user_pos->second.empty()) {
    //        std::map<uint16_t, boost::shared_ptr<User> >::iterator it;
    //        it =  user_pos->second.begin();
    //        return it->second;
    //    }

    //    boost::shared_ptr<User> null_client;
    //    return null_client;
    //}

    LOG(INFO) << "conn count :" << user_pos->second.size();
    std::map<uint16_t, boost::shared_ptr<User> >::iterator it;
    it = user_pos->second.find(port);
    if (it == user_pos->second.end()) {
        //struct in_addr in = {};
        //in.s_addr = htonl(ip);
        //std::string ip_str(inet_ntoa(in));
        //LOG(INFO) << "not found by port, user: " << ip_str
        //    << ", port: " << port;
        boost::shared_ptr<User> null_client;
        return null_client;
    }

    return it->second;
}

void Ship::Broadcast(const boost::shared_ptr<PushMsg> &push_msg)
{
    if (ordered_users_.empty()) {
        LOG(WARNING) << "there is no user to broadcast message";
        push_msg->Notify();
        return;
    }
    std::string  push_task_id = osl::StringUtil::valueOf(push_msg->push_task_id()) + osl::MD5::md5h(push_msg->request_str());
    if(broadcast_task_ids_.find(push_task_id) != broadcast_task_ids_.end()) {
        LOG(WARNING) << "duplicated task task id "<< push_task_id;
        return ;
    }
    broadcast_task_ids_[push_task_id] = time(NULL);

    boost::shared_ptr<BroadcastOperation> broadcast = boost::make_shared<BroadcastOperation>();
    broadcast->user_ = &(ordered_users_.front());
    CHECK_NOTNULL(broadcast->user_);
    broadcast->push_msg_ = push_msg;

    pending_operations_.push_back(broadcast);

    DoBroadcast(broadcast);

    return;
}

void Ship::Multicast(const boost::shared_ptr<PushMsg> &push_msg)
{
    if (ordered_users_.empty()) {
        LOG(WARNING) << "there is no user to push multicast message";
        push_msg->Notify();
        return;
    }

    boost::shared_ptr<MulticastOperation> cast = boost::make_shared<MulticastOperation>();

    cast->push_msg_ = push_msg;

    pending_multicast_operations_.push_back(cast);

    DoMulticast(cast);

    return;
}

void Ship::DoMulticast(boost::shared_ptr<MulticastOperation> &multicast)
{
    int once_push_users = gateway_->gateway_option().once_push_users;

    RepeatedClientID cids = multicast->push_msg_->cids();
    //loop
    for (int i = 0; i < once_push_users; i++) {
        uint32_t ip = cids.Get(multicast->index_).ipv4();
        uint16_t port = cids.Get(multicast->index_).port();
        boost::shared_ptr<User> user = GetUser(ip, port);
        if (!user) {
            DLOG(INFO) << "can not get user, may be offline";
            continue;
        }

        multicast->push_msg_->PushMsgToUser(user.get());
        multicast->index_++;
    }

    if (multicast->index_ >= cids.size()) {
        RemoveMulticastOperation(multicast);
    }
    else {
        //LOG(INFO) << "start one multicast task, once push users: " << once_push_users;
        event_loop_->QueueInLoop(boost::bind(&Ship::DoMulticast, this, multicast));
    }

}

boost::shared_ptr<ldd::net::Channel> Ship::GetServer(int msg_type, size_t *size)
{
    boost::shared_ptr<ldd::net::Channel> null_channel;

    ServiceIt service_pos = services_.find(msg_type);
    if (service_pos == services_.end()) {
        DLOG(ERROR) << "not found server for msg type: " << msg_type;
        return null_channel;
    }

    *size = service_pos->second->size();

    //note
    return service_pos->second->GetChannel();
}


bool Ship::AddUser(const boost::shared_ptr<ldd::net::Channel> channel)
{
    uint32_t ip = channel->peer_endpoint().address().ToU32();
    uint16_t port = channel->peer_endpoint().port();

    UserIt user_pos = users_.find(ip);
    if (user_pos == users_.end()) {
        //IP第一个连接
        std::map<uint16_t, boost::shared_ptr<User> > user_port_map;
        users_.insert(std::make_pair(ip, user_port_map));
        user_pos = users_.find(ip);
        if (user_pos == users_.end()) {
            LOG(ERROR) << "insert map<port:user> to users error, channel: "
                << channel->name();
            return false;
        }
    }

    boost::shared_ptr<User> user = boost::make_shared<User>(gateway_
                , channel
                , this
                , gateway_->user_option().first_packet_timeout);
    if (!user) {
        LOG(FATAL) << "create user failed";
        return false;
    }

    //note for session test, 测试完成后，可以把这段代码删掉
    //如果实现了login和sconf，这段代码不需要
    if (gateway_->test_option().enable) {
        //const user mid
        std::string mid(user->name());
        LOG(INFO) << "session test, user mid: " << mid;
        user->set_mid(mid);
        user->ship()->AddNewUser(user);
    }

    std::map<uint16_t, boost::shared_ptr<User> >::iterator pos
        = user_pos->second.find(port);
    if (pos != user_pos->second.end()) {
        LOG(ERROR) << "user has existed, old name: "
            << pos->second->name()
            << ", old channel addr:"
            << pos->second->channel().get()
            << ", old user addr: "
            << pos->second.get()
            << ", old user use count: "
            << pos->second.use_count()
            << ", new name: "
            << channel->name()
            << ", new channel addr:"
            << channel.get()
            << ", new user add: "
            << user.get()
            << ", new user use count: "
            << user.use_count();
        return false;
    }

    std::pair<std::map<uint16_t, boost::shared_ptr<User> >::iterator, bool> res
        = user_pos->second.insert(std::make_pair(port, user));
    if (!res.second) {
        LOG(ERROR) << "insert user error, name: "
            << channel->name()
            << ", channel addr:"
            << channel.get()
            << ", user addr: "
            << user.get()
            << ", user use count: "
            << user.use_count();
        return false;
    }

    ordered_users_.push_back(*user);
    channel->set_context(boost::weak_ptr<User>(user));

    user_count_++;

    //限制同一个ip的连接数
    if (user_pos->second.size() > (uint32_t)gateway_->user_option().max_connects) {
        //channel->Close(); //不能在这里直接关连接，需要给客户端返回错误码
        DLOG(WARNING) << "byond user connects limit: "
            << gateway_->user_option().max_connects;
        //return false;
        user->set_byond_connect_limit(true);
    }

    return true;
}

void Ship::RemoveUser(const boost::shared_ptr<ldd::net::Channel> &channel)
{
    uint32_t ip = channel->peer_endpoint().address().ToU32();
    UserIt user_pos = users_.find(ip);
    if (user_pos == users_.end()) {
        LOG(ERROR) << "remove user failed, not found user " 
            << channel->name()
            << " by ip: "
            << channel->peer_endpoint().address().ToString();
        return;
    }

    uint16_t port = channel->peer_endpoint().port();
    std::map<uint16_t, boost::shared_ptr<User> >::iterator pos;

    do {
        pos = user_pos->second.find(port);
        if (pos == user_pos->second.end()) {
            LOG(ERROR) << "remove user failed, not found user: "
                << channel->name()
                << " by port: "
                << port
                << ", channel addr: "
                << channel.get();

            break;
        }

        if (!pos->second->mid().empty()) {
            //只处理已经登录成功的用户
            //for session
            AddOldUser(pos->second);
        }

        ReviseBroadcastOperation(pos->second.get());
        ReviseSessionOperation(pos->second.get());

        //LOG(INFO) << "remove user: " << channel->name();

        pos->second->unlink();

        user_pos->second.erase(pos);
        user_count_--;

    } while (0);

    if (user_pos->second.empty()) {
        users_.erase(user_pos);
    }

#if 0
#ifdef _DEBUG
    int real_user_count = 0;
    UserIt t = users_.begin();
    for (; t!= users_.end(); t++) {
        real_user_count += t->second.size();
    }

    LOG(ERROR) << "user info with ship["
        << id()
        << "]: user ip count: "
        << users_.size()
        << ", users count: "
        << user_count_
        << ", real users: "
        << real_user_count
        << ", ordered users: "
        << ordered_users_.size();
#endif
#endif
}

void Ship::DoBroadcast(boost::shared_ptr<BroadcastOperation> &push)
{
    if (push->user_ == NULL) {
        LOG(WARNING) << "push task ended duriation pushing msg";
        RemoveBroadcastOperation(push);
        return;
    }

    if (!push->user_->is_linked()) {
        LOG(ERROR) << "user has unlinked, user addr: "
            << push->user_;
        RemoveBroadcastOperation(push);
        return;
    }

    UserList::iterator cur = UserList::s_iterator_to(*push->user_);

    int once_push_users = gateway_->gateway_option().once_push_users;

    //loop
    for (int i = 0; i < once_push_users && cur != ordered_users_.end() ; i++) {
        if(ZoneOrGrayLimit(push,&(*cur))) {
            cur++;
            --i;
            continue;
        }
        push->push_msg_->PushMsgToUser(&(*cur));
        cur++;
    }

    if (cur == ordered_users_.end()) {
        RemoveBroadcastOperation(push);
    }
    else {
        //LOG(INFO) << "start one push task, once push users: " << once_push_users;
        push->user_ = &(*cur);
        event_loop_->QueueInLoop(boost::bind(&Ship::DoBroadcast, this, push));
    }
}

bool Ship::ZoneOrGrayLimit(const boost::shared_ptr<BroadcastOperation> &push, User *user)const  {
/*{{{*/
    if(push->push_msg_->push().has_mid_gray()) {
        const std::string& mid = user->mid();
        uint64_t crc = osl::CRC32::getCRC32(mid.data(),mid.size());
        uint64_t max_value = push->push_msg_->push().mid_gray().max_value();
        crc = crc % max_value;
        if( crc < push->push_msg_->push().mid_gray().start()||
            crc >= push->push_msg_->push().mid_gray().end()) {
            //gray limited
            return true;
        }
    } else {
        DLOG(INFO) << "not has gray";
    }
    if(push->push_msg_->push().mid_zones().size() > 0) {
        if(!gateway_->ip_locate_query()) {
            return false;
        }
        safe::util::IpLocateQuery::const_iterator it = gateway_->ip_locate_query()->ipQuery(user->ip().data());
        DLOG(INFO) << "message :" << push->push_msg_->push().Utf8DebugString();
        DLOG(INFO) << "ip :" << it->Utf8DebugString();
        for(int i = 0; i < push->push_msg_->push().mid_zones().size(); ++i) {
            const lcs::Zone& zone = push->push_msg_->push().mid_zones(i);
            if(it == NULL) {
                break;//查询不到ip信息的不做多播
            }
            if(zone.has_province_name() && zone.province_name() != it->province_name()) {
                continue;
            }
            if(zone.has_city_name() && zone.city_name() != it->city_name()) {
                continue;
            }
            if(zone.has_zip_code() && zone.zip_code() != it->zip_code()) {
                continue;
            }
            if(zone.has_telephone_area_code() &&zone.telephone_area_code() != it->telephone_area_code()) {
                continue;
            }
            return false;
        }
        return true;
        //zone limited
    } else {
        DLOG(INFO) << "not has zone";
    }
    return false;
}/*}}}*/

void Ship::StartTimeCacheTimer()
{/*{{{*/
    time_cache_timer_.reset(new ldd::net::TimerEvent(event_loop_));
    time_cache_timer_->AsyncWait(
            boost::bind(&Ship::UpdateTimeCache, this),
            ldd::util::TimeDiff::Seconds(1));
}/*}}}*/


void Ship::StartSessionTimer()
{/*{{{*/
    session_timer_.reset(new ldd::net::TimerEvent(event_loop_));
    session_timer_->AsyncWait(
            boost::bind(&Ship::SessionTimerHandler, this),
            ldd::util::TimeDiff::Milliseconds(
                gateway_->session_option().user_check_interval));
}/*}}}*/


void Ship::SessionTimerHandler()
{/*{{{*/
    SendUserInfo();
    session_timer_->AsyncWait(
            boost::bind(&Ship::SessionTimerHandler, this),
            ldd::util::TimeDiff::Milliseconds(
                gateway_->session_option().user_check_interval));
}/*}}}*/



void Ship::UpdateTimeCache()
{
    //LOG(INFO) << "old time: " << time_cache_; 
    time_cache_ = time(NULL);
    //LOG(INFO) << "current time: " << time_cache_;
    std::map<std::string, time_t>::iterator it = broadcast_task_ids_.begin();
    for(; it != broadcast_task_ids_.end(); ) {
        if(time_cache_ - (it->second) > 300) {
            broadcast_task_ids_.erase(it++);
        } else {
            it++;
        }
    }

    time_cache_timer_->AsyncWait(
            boost::bind(&Ship::UpdateTimeCache, this),
            ldd::util::TimeDiff::Seconds(1));
}


void Ship::RemoveBroadcastOperation(const boost::shared_ptr<BroadcastOperation> &push)
{
    if (pending_operations_.empty()) {
        LOG(WARNING) << "pending broadcast operation is empty";
        return;
    }

    // remove from pending
    std::vector<boost::shared_ptr<BroadcastOperation> >::iterator it 
        = pending_operations_.begin();
    for (; it != pending_operations_.end(); it++) {
        if (*it == push) {
            //(*it)->push_msg_->Notify();
            pending_operations_.erase(it);
            //LOG(INFO) << "remove a push task";
            break;
        }
    }

    LOG(INFO) << "complete a push task, thread id: " << id();
}

void Ship::RemoveMulticastOperation(const boost::shared_ptr<MulticastOperation> &multicast)
{
    if (pending_multicast_operations_.empty()) {
        LOG(WARNING) << "pending multicast operation is empty";
        return;
    }

    //remove from pending
    std::vector<boost::shared_ptr<MulticastOperation> >::iterator it 
        = pending_multicast_operations_.begin();
    for (; it != pending_multicast_operations_.end(); it++) {
        if (*it == multicast) {
            //(*it)->push_msg_->Notify();
            pending_multicast_operations_.erase(it);
            //LOG(INFO) << "remove a multicast task";
            break;
        }
    }
    
    LOG(INFO) << "complete a multicast task, thread id: " << id();
}

void Ship::AddNode(const Node &node)
{
    //方便删除节点时查找
    std::pair<std::map<std::string, Node>::iterator, bool> res = 
        nodes_.insert(std::make_pair(node.path, node));
    if (!res.second) {
        //note
        //LOG(WARNING) << "repeated service node: " << node.path;
        return;
    }

    //LOG(INFO) << "add node: " << node.path;

    //create channel by nodes
    std::vector<int>::const_iterator protocol_it 
        = node.protocol.begin();
    for (; protocol_it != node.protocol.end(); protocol_it++) {

        //LOG(INFO) << "node protocol value: " << *protocol_it;

        //保存到services_中，客户端请求到来时
        std::map<int, boost::shared_ptr<Service> >::iterator s_it;
        s_it = services_.find(*protocol_it);
        if (s_it == services_.end()) {
            boost::shared_ptr<Service> s 
                = boost::make_shared<Service>(*protocol_it);
            services_.insert(std::make_pair(*protocol_it, s));
            s_it = services_.find(*protocol_it);
            if (s_it == services_.end()) {
                LOG(ERROR) << "add service error, service type: " 
                    << *protocol_it;
                return;
            }
        }

        std::vector<int>::const_iterator port_it = node.port.begin();
        for (; port_it != node.port.end(); port_it++) {

            //LOG(INFO) << "connect to " << node.ip << ":" << *port_it;
            boost::shared_ptr<ldd::net::Channel> channel 
                = client_->Create(
                        event_loop_
                        , node.ip
                        , *port_it);

            if (!channel) {
                LOG(ERROR) << "connect to server " 
                    << node.ip << ":" << *port_it << " failed";
                continue;
            }

            //for session
            channel->set_context(*protocol_it);

            gateway_->IncServers();
            s_it->second->AddChannel(channel);
        }
    }
}

void Ship::RemoveNode(const Node &node)
{
    std::map<std::string, Node>::iterator pos;
    pos = nodes_.find(node.path);
    if (pos == nodes_.end()) {
        //LOG(ERROR) << "not found : " << node.path;
        return;
    }

    DLOG(INFO) << "remove node: " << pos->second.host;

    std::vector<int>::const_iterator protocol_it; 
    for (protocol_it = pos->second.protocol.begin()
            ; protocol_it != pos->second.protocol.end()
            ; protocol_it++) {

        //note, need rewrite
        ServiceIt s_it = services_.find(*protocol_it);

        if (s_it == services_.end()) {
            LOG(WARNING) << "not found protocol: " << *protocol_it
                << " when remove node";
            continue;
        }

        std::vector<int>::const_iterator port_it = pos->second.port.begin();
        for (; port_it != pos->second.port.end(); port_it++) {

            //LOG(INFO) << "remove channel: "
            //    << pos->second.host
            //    << ", ip: "
            //    << pos->second.ip
            //    << ", protocol: "
            //    << *protocol_it
            //    << ", port: "
            //    << *port_it;

            gateway_->DecServers();
            s_it->second->RemoveChannel(pos->second.ip, *port_it);
        }
    }

    nodes_.erase(pos);
}

void Ship::ReviseBroadcastOperation(User *user)
{
    CHECK_NOTNULL(user);

    if (!user->is_linked()) {
        LOG(INFO) << "user has unlinked from pending operations";
        return;
    }

    //如果user在pending_operations中
    //那么将pending_operation中的指针指向user的下一个元素
    std::vector<boost::shared_ptr<BroadcastOperation> >::iterator it;
    for (it = pending_operations_.begin();
            it != pending_operations_.end();
            it++) {
        if ((*it)->user_ == user) {
            UserList::iterator ulit = UserList::s_iterator_to(*((*it)->user_));
            if (ulit == ordered_users_.end()) {
                (*it)->user_ = NULL;
                //LOG(INFO) << "the last user: " << user->name()
                //    << " user addr: " << user
                //    << " in pending operations";
                continue;
            }

            ulit++;

            if (ulit == ordered_users_.end()) {
                (*it)->user_ = NULL;
                //LOG(INFO) << "is the last user: " << user->name()
                //    << " user addr: " << user
                //    << " in pending operations";
                continue;
            }

            //LOG(INFO) << "before ++, user addr: "
            //    << (*it)->user_
            //    << ", after ++, user addr: "
            //    << &(*ulit);

            (*it)->user_ = &(*ulit);
        }
    }
}

uint64_t Ship::CurrentUsers()
{
    return ordered_users_.size();
}

void Ship::AddNewUser(const boost::shared_ptr<User> &user)
{
    new_users_.insert(std::make_pair(
                std::make_pair(user->ip_u32(), user->port()), user->mid())
            );
    CheckUserCount();
}

void Ship::AddOldUser(const boost::shared_ptr<User> &user)
{
    if (user->mid().empty()) {
        LOG(INFO) << "invalie user mid, user: "
            << user->ip()
            << ":"
            << user->port();
        return;
    }

    old_users_.insert(std::make_pair(
                std::make_pair(user->ip_u32(), user->port()), user->mid())
            );
    CheckUserCount();
}

void Ship::CheckUserCount()
{
    int user_count = 0;
    if (!new_users_.empty()) {
        user_count += new_users_.size();
    }

    if (!old_users_.empty()) {
        user_count += old_users_.size();
    }

    if (user_count >= gateway_->session_option().user_num_once_send) {
        SendUserInfo();
    }
}


void Ship::SendUserInfo()
{
    SendUserOnlineInfo();
    SendUserOffLineInfo();

}
void Ship::RecordPushSessionInfo(const boost::shared_ptr<ldd::net::Channel> &channel, lcs::srv::Sessions& sessions) {
    for(int i = 0; i < sessions.sessions().size(); ++i) {
        const std::string& mid =  sessions.sessions(i).mid();
        uint32_t ipv4 = sessions.sessions(i).channel().cid().ipv4();
        uint16_t port = sessions.sessions(i).channel().cid().port();
        user_login_push_channel_[std::make_pair(ipv4, port)] = channel;
        push_channel_login_users_[channel.get()].insert(std::make_pair(
                    std::make_pair(ipv4, port), mid)
                );
    }
}



void Ship::ServerConnected(const boost::shared_ptr<ldd::net::Channel>& channel)
{
    if (channel->context().empty()) {
        LOG(FATAL) << channel->name()
            << ", invalid server context";
        return;
    }

    //note
    //int protocol = boost::any_cast<int>(channel->context());
    //if (protocol == 1200) {
    //    //如果是session server, 发送所有的用户消息到服务器
    //    //创建一个session 发送任务
    //    LOG(INFO) << "server type: push(1200), send all user info to session server";
    //    CreateSessionOperation(channel);
    //}

}
void Ship::ServerClosed(const boost::shared_ptr<ldd::net::Channel>& channel)
{
    LOG(INFO) << "to server closed" << channel.get();
    std::map<std::pair<uint32_t, uint16_t>, std::string>& channel_users = push_channel_login_users_[channel.get()];
    std::map<std::pair<uint32_t, uint16_t>, std::string>::const_iterator it = channel_users.begin();
    if(!channel_users.empty()) {
        LOG(INFO) << "one session server closed report user sessions to other server";
    }
    for(; it != channel_users.end(); ++it) {
        new_users_[it->first] = it->second;
    }
    push_channel_login_users_.erase(channel.get());

}

void Ship::SendUserOnlineInfo() {
    //find push server
    std::map<int, boost::shared_ptr<Service> >::iterator service_pos = services_.find(1200);
    if (service_pos == services_.end()) {
        //LOG(INFO) << "no push server to be used when send user mid";
        return;
    }

    boost::shared_ptr<Service> sess_srv = service_pos->second;
    if (!sess_srv) {
        LOG(FATAL) << "invalid session server pointer";
        return;
    }

    if (sess_srv->size() <= 0) {
        LOG(ERROR) << "no session service";
        return;
    }
    int server_count = sess_srv->size();

    boost::shared_ptr<ldd::net::Channel> channel ;
    for (int i = 0; i < server_count; i++) {
        channel = sess_srv->GetChannel();
        if(channel) {
            break;
        }
    }
    if(!channel) {
        return ;
    }
    if(new_users_.size() == 0) {
        return ;
    }

    boost::shared_ptr<SessionOperation> sess = boost::make_shared<SessionOperation>();

    sess->ordered_user_sessions_.swap(new_users_);

    sess->channel_ = channel;

    sess->status = kOnline;

    pending_session_operations_.push_back(sess);


    sess->cur_user_ = sess->ordered_user_sessions_.begin();

    DoSessionOperation(sess);

    return;

}
void Ship::SendUserOffLineInfo() {
    std::map<ldd::net::Channel*, boost::shared_ptr<SessionOperation> > sess_op_map;
    UserSessionMap::iterator it   = old_users_.begin();
    UserSessionMap::iterator ite  = old_users_.end();
    for(; it != ite; ) {
        boost::shared_ptr<ldd::net::Channel> channel = user_login_push_channel_[it->first];

        new_users_.erase(it->first);

        user_login_push_channel_.erase(it->first);
        push_channel_login_users_[channel.get()].erase(it->first);
        if(!channel) {
            old_users_.erase(it++);
            continue;
        }        
        boost::shared_ptr<SessionOperation>& sess_op = sess_op_map[channel.get()];
        if(!sess_op) {
            sess_op.reset(new SessionOperation);
            sess_op->status = kOffline;
            sess_op->channel_ = channel;
        }
        sess_op->ordered_user_sessions_[it->first] = it->second;
        it++;
    }
    std::map<ldd::net::Channel*, boost::shared_ptr<SessionOperation> >::const_iterator op_it = sess_op_map.begin();
    for(; op_it != sess_op_map.end(); ++op_it) {
        DLOG(INFO) << "send user offline info";
        boost::shared_ptr<SessionOperation> sess_op = op_it->second;
        sess_op->cur_user_ = sess_op->ordered_user_sessions_.begin();
        pending_session_operations_.push_back(sess_op);
        DoSessionOperation(sess_op);
    }
    old_users_.clear();

}

void Ship::DoSessionOperation(boost::shared_ptr<SessionOperation> &sess_op)
{
    //LOG(INFO) << "do session operation";
    if (sess_op->cur_user_ == sess_op->ordered_user_sessions_.end()) {
        LOG(ERROR) << "session task ended duriation pushing msg";
        RemoveSessionOperation(sess_op);
        return;
    }

    UserSessionMap::const_iterator& cur = sess_op->cur_user_;

    int user_num_once_send = gateway_->session_option().user_num_once_send;

    //loop
    lcs::srv::Sessions sessions;
    for (int i = 0; i < user_num_once_send && cur != sess_op->ordered_user_sessions_.end() ; i++) {
        lcs::srv::Session *sess = sessions.add_sessions();
        sess->set_mid(cur->second);
        sess->set_status(sess_op->status);

        sess->mutable_channel()->mutable_cid()->set_ipv4(cur->first.first);
        sess->mutable_channel()->mutable_cid()->set_port((uint32_t)cur->first.second);
        if(sess->mid().size() == 32) {
            DLOG(INFO) << "send user session info " 
                <<"\t mid:" << sess->mid()
                <<"\t status:" <<sess->status(); 
        } else {
            DLOG(INFO) << "mid size error:" << sess->mid().size(); 
        }

        cur++;
    }

    if (sessions.sessions().size() > 0) {
         LOG(INFO) << "report session size:" << sessions.sessions().size() ;
        //发送给push server,push和session是在一起的。
        std::string session_str;
        if (!sessions.SerializeToString(&session_str)) {
            LOG(ERROR) << "serialize sessions to string error";
            return;
        }

        if(!SendSession(session_str,sess_op->channel_,0)) {
            RemoveSessionOperation(sess_op);
            return ;
        }
        if(sess_op->status == kOnline) {
            RecordPushSessionInfo(sess_op->channel_,sessions);
        }

    }

    if (cur == sess_op->ordered_user_sessions_.end()) {
        RemoveSessionOperation(sess_op);
        return;
    }

    //LOG(INFO) << "start one session task, user num one send: " << user_num_once_send;
    event_loop_->QueueInLoop(boost::bind(&Ship::DoSessionOperation, this, sess_op));
}

bool Ship::SendSession(const std::string& msg_content, boost::shared_ptr<ldd::net::Channel> channel, int retried_times) {
    if(!channel) {
        return false;
    }
    if(retried_times >= gateway_->session_option().max_retry_counts) {
        return false;
    }
    boost::shared_ptr<ToSession> msg 
        = boost::make_shared<ToSession>(100
                , ldd::net::Buffer(msg_content)
                , gateway_->session_option().recv_timeout
                , retried_times
                , this);
    if (!channel->Post(msg
                , ldd::util::TimeDiff::Milliseconds(
                    gateway_->session_option().send_timeout))) {
        LOG(ERROR) << "send session msg to server " 
            << channel->name() << " failed";
        return false;
    }
    return true;

}
void Ship::RemoveSessionOperation(const boost::shared_ptr<SessionOperation> &sess)
{
    if (pending_session_operations_.empty()) {
        LOG(WARNING) << "pending session operation is empty";
        return;
    }

    // remove from pending
    std::vector<boost::shared_ptr<SessionOperation> >::iterator it 
        = pending_session_operations_.begin();
    for (; it != pending_session_operations_.end(); it++) {
        if (*it == sess) {
            pending_session_operations_.erase(it);
            break;
        }
    }
    
    LOG(INFO) << "complete a session task, thread id: " << id();
}

void Ship::ReviseSessionOperation(User *user)
{

    //CHECK_NOTNULL(user);

    //if (!user->is_linked()) {
    //    LOG(INFO) << "user has unlinked from pending session operations";
    //    return;
    //}

    //如果user在pending_operations中
    //那么将pending_operation中的指针指向user的下一个元素
    std::vector<boost::shared_ptr<SessionOperation> >::iterator it;
    UserKey user_ip_port = std::make_pair(user->ip_u32(), user->port());
    for (it = pending_session_operations_.begin();
            it != pending_session_operations_.end();
            it++) {
        boost::shared_ptr<SessionOperation> pending_session_operation = *it;
        if(pending_session_operation->cur_user_ == pending_session_operation->ordered_user_sessions_.end()) {
            continue;
        }
        if(pending_session_operation->cur_user_->first == user_ip_port) {
            pending_session_operation->cur_user_++;
        }
        pending_session_operation->ordered_user_sessions_.erase(user_ip_port);
    }
}

} /*gateway*/
} /* lcs */

