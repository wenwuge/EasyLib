// ship.h (2013-09-13)
// Yan Gaofeng (yangaofeng@360.cn)
//
#ifndef LCS_GATEWAY_SHIP_H_
#define LCS_GATEWAY_SHIP_H_

#include <boost/intrusive/list.hpp>
#include <boost/shared_ptr.hpp>
#include "proto/lcs_session.pb.h"

#include "config.h"
#include "push_msg.h"
#include "user.h"
#include "service.h"

namespace lcs { namespace gateway {

class GateWayModule;

typedef boost::intrusive::list<User, boost::intrusive::constant_time_size<false> > UserList;

typedef std::map<int, std::map<uint16_t, boost::shared_ptr<User> > > UserMap;

typedef std::pair<uint32_t, uint16_t> UserKey;

typedef std::map<UserKey, std::string> UserSessionMap;


struct BroadcastOperation {
    BroadcastOperation() : user_(NULL) {}
    ~BroadcastOperation() {}

    User* user_;
    boost::shared_ptr<PushMsg> push_msg_;
};

struct MulticastOperation {
    MulticastOperation() : index_(0) {}
    ~MulticastOperation() {}

    int index_;
    boost::shared_ptr<PushMsg> push_msg_;
};

struct SessionOperation {
    SessionOperation() {}
    ~SessionOperation() {}


    UserSessionMap::const_iterator cur_user_;
    UserSessionMap ordered_user_sessions_;
    boost::shared_ptr<ldd::net::Channel> channel_;
    ServerStatus status;

};

struct UserInfo {
    UserInfo(uint32_t user_ip, uint16_t user_port, const std::string user_mid)
    : ip(user_ip), port(user_port), mid(user_mid) { }
    uint32_t ip;
    uint16_t port;
    std::string mid;
};


typedef std::map<int, boost::shared_ptr<Service> >::iterator ServiceIt;
typedef std::map<std::string, std::list<Node> >::iterator NodeIt;
typedef std::map<int, std::map<uint16_t, boost::shared_ptr<User> > >::iterator UserIt;


class Ship {
public:
    explicit Ship(GateWayModule *gateway, ldd::net::EventLoop *event_loop);
    ~Ship();

    boost::shared_ptr<ldd::net::Channel> GetServer(int msg_type, size_t *size);

    boost::shared_ptr<User> GetUser(uint32_t ip, uint16_t port);
    bool AddUser(const boost::shared_ptr<ldd::net::Channel> channel);
    void RemoveUser(const boost::shared_ptr<ldd::net::Channel> &channel);
    uint64_t CurrentUsers();

    void AddNode(const Node &node);
    void RemoveNode(const Node &node);


    void Broadcast(const boost::shared_ptr<PushMsg> &push_msg);
    void Multicast(const boost::shared_ptr<PushMsg> &push_msg);
    void DoMulticast(boost::shared_ptr<MulticastOperation> &multicast);
    void RemoveMulticastOperation(const boost::shared_ptr<MulticastOperation> &multicast);

    pthread_t id() { return id_; }
    void set_id(pthread_t id) { id_ = id; }
    time_t time_cache() { return time_cache_; };

    ldd::net::EventLoop *event_loop() { return event_loop_; }

    void RemoveBroadcastOperation(const boost::shared_ptr<BroadcastOperation> &broadcast);

    //for session
    void AddNewUser(const boost::shared_ptr<User> &user);
    void AddOldUser(const boost::shared_ptr<User> &user);
    void StartSessionTimer();
    bool SendSession(const std::string& msg_content, boost::shared_ptr<ldd::net::Channel> channel, int retried_times); 

private:


    void StartTimeCacheTimer();
    void UpdateTimeCache();
    bool ZoneOrGrayLimit(const boost::shared_ptr<BroadcastOperation> &push, User *user) const; 

    //for push
    void DoBroadcast(boost::shared_ptr<BroadcastOperation> &broadcast);
    void ReviseBroadcastOperation(User *user);

    //for session
    void DoSessionOperation(boost::shared_ptr<SessionOperation> &sess);
    void RemoveSessionOperation(const boost::shared_ptr<SessionOperation> &sess);
    void ReviseSessionOperation(User *user);
    void ServerConnected(const boost::shared_ptr<ldd::net::Channel>& channel);
    void ServerClosed(const boost::shared_ptr<ldd::net::Channel>& channel);
    void CheckUserCount();
    void SendUserInfo();
    void SendUserOnlineInfo();
    void SendUserOffLineInfo();
    void RecordPushSessionInfo(const boost::shared_ptr<ldd::net::Channel> &channel, lcs::srv::Sessions& sessions);
    void SessionTimerHandler();

    unsigned int cal_crc(unsigned int crc, const unsigned char *ptr, unsigned char len) const;
    unsigned int cal_crc(const unsigned char *ptr, unsigned char len) const ;
private:
    friend class GateWayModule;
    friend class User;

    ldd::net::EventLoop *event_loop_;

    //connect to server, one thread one client
    boost::shared_ptr<ldd::net::Client> client_;

    //for request limit peer second
    UserList ordered_users_;

    //users, ip as key, 为了支持连接数限制,使用list
    UserMap users_;

    //for session
    std::map<UserKey, std::string> new_users_;
    std::map<UserKey, std::string> old_users_;
    boost::scoped_ptr<ldd::net::TimerEvent> session_timer_;

    //services, int protocol as key
    std::map<int, boost::shared_ptr<Service> > services_;

    //for broadcast msg,记录正在进行的推送任务，如果有期间有客户端断开连接
    //将pending_operations_中的元素后移
    std::vector<boost::shared_ptr<BroadcastOperation> > pending_operations_;

    //for multicast msg
    std::vector<boost::shared_ptr<MulticastOperation> > pending_multicast_operations_;

    //for session
    std::vector<boost::shared_ptr<SessionOperation> > pending_session_operations_;

    boost::scoped_ptr<ldd::net::TimerEvent> time_cache_timer_;
    time_t time_cache_;

    pthread_t id_;

    GateWayModule *gateway_;

    //std::string path, such as: /lcs/bjdt/kill/192.168.1.1
    std::map<std::string, Node> nodes_;
    std::map<UserKey, boost::shared_ptr<ldd::net::Channel> > user_login_push_channel_ ;
    std::map<ldd::net::Channel*, std::map<UserKey, std::string> > push_channel_login_users_;

    std::map<std::string, time_t> broadcast_task_ids_;

    uint64_t user_count_;
};

} /*gateway*/
} /*lcs*/


#endif // LCS_GATEWAY_SHIP_H_
