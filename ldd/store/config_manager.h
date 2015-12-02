
#ifndef _LDD_STORAGE_CONFIG_MANAGER_H_
#define _LDD_STORAGE_CONFIG_MANAGER_H_


#include <map>
#include <vector>
#include <set>
#include <string>
#include <boost/shared_ptr.hpp>
#include <ldd/util/atomic.h>
#include <ldd/util/mutex.h>

#include "ldd/courier/courier.h"
#include "ldd/courier/keeper.h"

#include "ldd/net/channel.h"
#include "ldd/net/event_loop.h"
#include "ldd/net/client.h"




using namespace ldd::courier;
using namespace ldd::net;


namespace ldd{
namespace storage{

class VersionInfoMessageS;
class GetVersionContext;
class DB;

class ConfigManager{
public:
    ConfigManager(EventLoop* el, Client::Options& opt, 
                    const std::string& home_host_port,
                    bool force_consistency,
                    int recv_timeout,
                    int done_timeout,
                    int prepare_interval);
    ~ConfigManager();

public:
    int  SetUpCourier(Options& option, std::string& courier_host_port, 
                        std::string& courier_root_path);
    void VersionInfoCallBack(std::string& host_port, int64_t version);
    std::string& GetHomeHostPort();
  /*  EventLoop* GetEventLoop(){
        return event_loop_;
    };*/

    typedef struct NT{
        NT():have_echo(0), version(0), echo_time(0){
            channel.reset();
           // context.reset();
        }
        std::string                    host_port;
        uint8_t                        have_echo;
        int64_t                        version;
        uint64_t                    echo_time;
        boost::shared_ptr<Channel>    channel;
       // boost::shared_ptr<GetVersionContext>            context;
    }Notification;

    /*
       for message from store to store
       */
    const std::vector<boost::shared_ptr<Channel> >* GetSlaveChannels();
    bool IsMaster();
    const std::set<uint16_t>& GetBucketIdSet();
    void SetDb(DB* db_impl){
        db_impl_ = db_impl;
    }

    DB* GetDb(){
        return db_impl_;
    }

    enum RoleType{
        kTypeFromSlave = 1,
        kTypeFromMaster= 2
    }; // for uint8_t

    RoleType GetFromTypeofMaster(){
        return kTypeFromMaster;
    }

    bool IsFromMaster(uint8_t e ){
        if ((RoleType)e == kTypeFromMaster){
            return true;
        }
        return false;
    }

    enum SelectionType{
        kNull,
        kOnSelecting,    // in selecting
        kDone,            // a selection finished, may create a not ok roles version
        kPerfect,        // system can server for read and write
    };
    bool IsReady();

    int get_recv_timeout(){
        return recv_timeout_;
    }

    int get_done_timeout(){
        return done_timeout_;
    }

    void SetPrepareTime(int64_t t);
    bool NeedDoPrepare();

private:
    ConfigManager(const ConfigManager&);
    ConfigManager& operator=(const ConfigManager&);

    void GetRolesHandler(int err_code, Roles& roles);
    void GotLeaderShipHandler(int err_code, bool got_leadership);
    void AliveNodeChangedHandler(int err_code, NodeChangedType type, 
                                const std::string& node);

    bool CheckHistoryRoles();
    void ClearChannels();
    void CreateChannels();
    void StartAssignRoles();
    void DoAssignRoles();
    bool AssignSlaveChannel();
    static bool Compare(ConfigManager::Notification a, 
                        ConfigManager::Notification b);

    
private:
    Keeper* keeper_ ; 
    std::string courier_host_port_;        
    std::string courier_root_path_;        

    int replica_size_;
    int binlog_size_; 
    int bucket_size_;
    std::string store_engine_;
    bool IsCoordinator_;
    ldd::util::Mutex mutex_;
    ldd::util::CondVar cond_;
    ldd::util::Atomic<SelectionType> selection_type_;

    static Roles roles_;
    std::string home_host_port_;        

    std::vector<std::string> alive_node_vec_;                   // host_port
    std::map<std::string, ConfigManager::Notification> alive_node_map_; // key = host_port 
    std::vector<boost::shared_ptr<Channel> > slave_channels_;
    ldd::util::Atomic<bool> slave_channel_set_;

    size_t version_response_size_;
    Client::Options option_;
    Client* client_;
    EventLoop* event_loop_;
    DB* db_impl_;

    bool is_master_;

    bool force_consistency_;
    int recv_timeout_;
    int done_timeout_ ;

    util::Atomic<int64_t> last_prepared_MilliSecond_;
    int64_t prepare_interval_;
};    

}//namespace storage
}//namespace ldd


#endif // _LDD_STORAGE_CONFIG_MANAGER_H_

