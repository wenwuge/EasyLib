
#include <string>
#include <glog/logging.h>
#include <boost/bind.hpp>

#include "ldd/util/time.h"
#include "context.h"
#include "config_manager.h"



namespace ldd{
namespace storage{


Roles ConfigManager::roles_;
ConfigManager::ConfigManager(EventLoop* el,Client::Options& opt,
                             const std::string& home_host_port,
                             bool force_consistency,
                             int recv_timeout,
                             int done_timeout,
                             int prepare_interval) 
    : keeper_(NULL), replica_size_(0), 
    binlog_size_(0), bucket_size_(0), 
    IsCoordinator_(false), cond_(&mutex_), 
    selection_type_(kNull),home_host_port_(home_host_port), 
    slave_channel_set_(false),version_response_size_(0),
    option_(opt),
    is_master_(false),
    force_consistency_(force_consistency),
    recv_timeout_(recv_timeout),
    done_timeout_(done_timeout),
    last_prepared_MilliSecond_(util::Time::CurrentMilliseconds()),
    prepare_interval_(prepare_interval){

    client_ = new net::Client(option_);
    event_loop_ = el;
}

ConfigManager::~ConfigManager() {
    delete keeper_;
    delete client_;
    LOG(WARNING)<<"ConfigManager::~ConfigManager()";
}

void ConfigManager::SetPrepareTime(int64_t t){
    last_prepared_MilliSecond_ = t;
}

bool ConfigManager::NeedDoPrepare(){
    int64_t now = util::Time::CurrentMilliseconds();
    if (now - last_prepared_MilliSecond_ > prepare_interval_){
        DLOG(INFO)<<"ConfigManager::NeedDoPrepare() now="<<now<<" last="<<last_prepared_MilliSecond_
            <<" interval="<<prepare_interval_<<" NeedDoPrepare!!";
        return true;
    }else{
        DLOG(INFO)<<"ConfigManager::NeedDoPrepare() now="<<now<<" last="<<last_prepared_MilliSecond_
            <<" interval="<<prepare_interval_<<" no prepare!!";
        return false;
    }
}

const std::set<uint16_t>& ConfigManager::GetBucketIdSet(){
    return keeper_->GetBucketSet();
}

std::string& ConfigManager::GetHomeHostPort(){
    return home_host_port_;
}

void ConfigManager::VersionInfoCallBack(std::string& host_port, int64_t version){
    std::map<std::string ,  Notification>::iterator itr = 
                                        alive_node_map_.find(host_port);
    if (itr!=alive_node_map_.end()){
        ConfigManager::Notification* n =  &(itr->second);
        n->have_echo    = 1;
        n->version        = version;
        n->echo_time    = ldd::util::Time::CurrentMilliseconds();
        LOG(WARNING)<<"ConfigManager::VersionInfoCallBack() version="
                    <<version<<" response times="<<version_response_size_;
    }else{
       LOG(ERROR)<<"ConfigManager::VersionInfoCallBack() error callback host_port="
           <<host_port<<" exit(3)";
       exit(3);
    }
    ++version_response_size_;
    
    if (alive_node_map_.size() == version_response_size_){
        LOG(WARNING)<<"ConfigManager::VersionInfoCallBack() DoAssignRoles()"
                    <<" all response comming ="<<version_response_size_;
        DoAssignRoles();    // 如果返回无效的结果，其实不能分配角色；
    }
}

bool ConfigManager::Compare(ConfigManager::Notification a, 
                            ConfigManager::Notification b){
    LOG(WARNING)<<"a.version="<<a.version<<" b.version="<<b.version;
    if (a.version > b.version){
        return true;
    }else if (a.version == b.version){
        if (a.host_port.compare(roles_.master)  == 0){
            LOG(WARNING)<<"a.host_port="<<a.host_port
                <<" roles_.master="<<roles_.master;
            return true;
        }
        if (b.host_port.compare(roles_.master)  == 0){
            LOG(WARNING)<<"b.host_port="<<b.host_port
                <<" roles_.master="<<roles_.master;
            return false;
        }
        for (size_t i=0; i<roles_.slaves.size(); i++){
            if (a.host_port.compare(roles_.slaves[i]) == 0){
                LOG(WARNING)<<"a.host_port="<<a.host_port
                    <<" b.version="<<b.host_port;
                return true;
            }
        }

        for (size_t i=0; i<roles_.slaves.size(); i++){
            if (b.host_port.compare(roles_.slaves[i]) == 0){
                LOG(WARNING)<<"a.host_port="<<a.host_port
                    <<" b.version="<<b.host_port;
                return false;
            }
        }
                                
        if (a.echo_time < b.echo_time){
            return true;
        }
    }else{
        return false;
    }
    return true;
}

void ConfigManager::DoAssignRoles(){
    //开始比较
    std::vector<ConfigManager::Notification> version_vec;
    std::map<std::string ,  ConfigManager::Notification>::const_iterator itr 
                                                    = alive_node_map_.begin();
    for (; itr!= alive_node_map_.end(); itr++){
        if (itr->second.have_echo == 1){
            const ConfigManager::Notification& n = itr->second;
            version_vec.push_back(n);
            LOG(WARNING)<<"ConfigManager::DoAssignRoles() have echo version host_port="
                <<itr->first<<" version="<<n.version;
        }else{
            LOG(ERROR)<<"ConfigManager::DoAssignRoles() no echo from host<<"<<itr->first;   // todo should ReAssignRoles
        }
    }
    
    if (version_vec.size() ==0 ){
        LOG(ERROR)<<"ConfigManager::DoAssignRoles(), error, need ReAssigne roles"
                  <<" alive_node_map_.size()"<<alive_node_map_.size();
        selection_type_ = kDone;
        return;
    }

    std::sort(version_vec.begin(), version_vec.end(), ConfigManager::Compare);
    
    roles_.master    = version_vec[0].host_port;
    LOG(WARNING)<<"ConfigManager::DoAssignRoles()  choose the master="
                <<roles_.master<<" total size="<<version_vec.size();

    roles_.slaves.clear();
    roles_.followers.clear();

    int slave_size        = 0;
    int follower_size    = 0;
    size_t index=1;
    for (index=1; index < version_vec.size() && 
                    index < (size_t)replica_size_; index++){
        if (force_consistency_ 
            && version_vec[index].version < version_vec[0].version){
            LOG(WARNING)<<"ConfigManager::DoAssignRoles() version "
                    <<version_vec[index].version<<" master version="
                    <<version_vec[0].version;
            break;
        }
        roles_.slaves.push_back(version_vec[index].host_port);
        slave_size++;
        LOG(WARNING)<<"ConfigManager::DoAssignRoles() choose the slave="
                 <<version_vec[index].host_port<<" index="<<index;
    }
    
    for (/*index=1*/; index<version_vec.size(); index++){
        follower_size++;
        roles_.followers.push_back(version_vec[index].host_port);
        LOG(WARNING)<<"ConfigManager::DoAssignRoles() choose the follower="
                <<version_vec[index].host_port<<" index="<<index;
    }
    
    LOG(WARNING)<<"ConfigManager::DoAssignRoles() slave size="
            <<slave_size<<" follower size="<<follower_size;

    ldd::util::MutexLock l(&mutex_);
    keeper_->AddNewRoles(roles_);
    if (slave_size + 1 < replica_size_){
        LOG(WARNING)<<" ConfigManager::DoAssignRoles() slave not enough!!! system can only server for read";
        selection_type_ = kDone;
    }else{                                        
        selection_type_ = kPerfect;
        LOG(WARNING)<<"ConfigManager::DoAssignRoles()  OK!!! system election finished";
    }
    cond_.Signal();    
}

bool ConfigManager::CheckHistoryRoles(){
    bool go_ahead = false;
    if (roles_.master.empty()){
        LOG(WARNING)<<"ConfigManager::CheckHistoryRoles() first time assign roles";
        go_ahead = true;
        return go_ahead;
    }

    for (size_t i=0; i<alive_node_vec_.size(); i++){
        std::string& host_port =alive_node_vec_[i];
        if (roles_.master.compare(host_port) == 0){
            go_ahead = true;
            LOG(WARNING)<<"ConfigManager::CheckHistoryRoles()  master="
                <<roles_.master<<" host_port="<<host_port;
            return go_ahead;
        }// if

        for (size_t j=0; j<roles_.slaves.size(); j++){
            std::string& s =roles_.slaves[j];
            if (s.compare(host_port) == 0){
                go_ahead = true;
                LOG(WARNING)<<"ConfigManager::CheckHistoryRoles() slave="<<s
                    <<" host_port="<<host_port;
                return go_ahead;
            }// if
        }//for    
    }// for
    return go_ahead;
}

void ConfigManager::StartAssignRoles(){
    /*ldd::util::MutexLock l(&mutex_);
    while (selection_type_ == kOnSelecting){
        LOG(WARNING)<<"ConfigManager::StartAssignRoles() a selection is in processing, wait()";
        cond_.Wait();
    }*/ 

    if(!CheckHistoryRoles()){
        LOG(WARNING)<<"ConfigManager::CheckHistoryRoles() not go ahead,no master in the history";
        return;
    }

    selection_type_ = kOnSelecting;  // this state must be set after CheckHistoryRoles()
    

    LOG(WARNING)<<"ConfigManager::StartAssignRoles()";
    
    ClearChannels();
    CreateChannels();    

    // 需要重新获取所有node的版本号
    LOG(WARNING)<<"ConfigManager::StartAssignRoles()";
    version_response_size_    = 0;
    std::map<std::string, Notification >::iterator itr = alive_node_map_.begin();
    for (; itr != alive_node_map_.end(); itr++){
        Notification& n = itr->second;
        boost::shared_ptr<GetVersionContext> 
            context(new GetVersionContext(this, home_host_port_));
        //n.context    = context;

        LOG(WARNING)<<"ConfigManager::StartAssignRoles() send message to get version, to address="<<n.host_port;
        n.channel->Post(context, util::TimeDiff::Milliseconds(200), true); 
    }// for
}

void ConfigManager::ClearChannels(){
    LOG(WARNING)<<"ConfigManager::ClearChannels(), map size="<<alive_node_map_.size();
    std::map<std::string ,  ConfigManager::Notification>::iterator itr2 = 
                                alive_node_map_.begin();
    while (itr2 != alive_node_map_.end()){
        ConfigManager::Notification& n = (itr2->second);
        n.channel->Close();
        n.channel.reset();         
        LOG(WARNING)<<"ConfigManager::ClearChannels() delete channel of host_port="
            <<n.host_port;
        alive_node_map_.erase(itr2++);        //++先指向下一个
    }
}

static bool GetHostPost(const std::string& host_port, 
                        std::string& host, int & port){
    size_t pos = host_port.find(':');
    LOG(WARNING)<<"ConfigManager::GetHostPost() host_port="<<host_port<<" pos="<<pos;
    if (pos == std::string::npos){
        LOG(ERROR)<<"ConfigManager::GetHostPost(), error host_port:"<<host_port;
        return true;
    }
    std::string strPos(host_port.begin()+pos+1, host_port.end());
    port = atoi(strPos.data());
    if (port <= 0){
        LOG(ERROR)<<"ConfigManager::GetHostPost(), error host_port:"<<host_port;
        return false;
    }

    host.assign(host_port.begin(), host_port.begin()+pos);
    LOG(WARNING)<<"ConfigManager::GetHostPost() host="<<host<<" port="<<port;
    return true;
}

void ConfigManager::CreateChannels(){
    LOG(WARNING)<<"ConfigManager::CreateChannels() host port size="
                                <<alive_node_vec_.size();
    if (alive_node_map_.size() > 0){
        LOG(WARNING)<<"ConfigManager::CreateChannels() already created map size="
                                <<alive_node_map_.size();
        return;
    }
    
    for (size_t i=0; i<alive_node_vec_.size(); i++){
        std::string& host_port =alive_node_vec_[i];
        std::string host;
        int port = 0;
        if (!GetHostPost(host_port, host, port)){
            LOG(ERROR)<<"ConfigManager::CreateChannels(), error host_port:"
                                <<host_port;
            return;
        }
        // only coordinator do this
        boost::shared_ptr<Channel> channel = 
            client_->Create(event_loop_, host, port);
        if (channel.get() == NULL){  // 判断是否合理
            LOG(ERROR)<<"ConfigManager::CreateChannels() error to connect host_port="
                                    <<host_port;
            return;
        }

        LOG(WARNING)<<"ConfigManager::CreateChannels() create connection successfully host="
                                <<host<<" port="<<port;
        std::pair<std::map<std::string, ConfigManager::Notification >::iterator, 
                    bool> insert_pair;
        ConfigManager::Notification notif; 
        notif.host_port    = host_port;
        notif.channel    = channel;
        insert_pair = alive_node_map_.insert(
                   std::make_pair<std::string, Notification >(host_port, notif));
        if (insert_pair.second == false){
            LOG(ERROR)<<"ConfigManager::CreateChannels() error to insert map, host_port="
                <<host_port;
        }else{
            LOG(WARNING)<<"ConfigManager::CreateChannels() succeed insert map, host_port="
                <<host_port;    
        }
    }// for
}


const std::vector<boost::shared_ptr<Channel> >* ConfigManager::GetSlaveChannels(){
    if (slave_channel_set_.Load()){
        return &slave_channels_;
    }else{
		LOG(WARNING)<<"GetSlaveChannels() NULL";
        return NULL;
    }
}

bool ConfigManager::AssignSlaveChannel(){    
    std::map<std::string,  Notification>::iterator itr;
    LOG(WARNING)<<"ConfigManager::GetSlaveChannels, slave size="
        <<roles_.slaves.size();
    for (size_t i=0; i<roles_.slaves.size(); i++){
        itr  = alive_node_map_.find(roles_.slaves[i]);
        if (itr == alive_node_map_.end()){
            LOG(ERROR)<<"ConfigManager::GetSlaveChannels() error not find channel for slave="
                            <<roles_.slaves[i];
            return false;
        }
        ConfigManager::Notification & n = itr->second;
        slave_channels_.push_back(n.channel);
        LOG(WARNING)<<"ConfigManager::GetSlaveChannels() add a channel(address)="
                            <<roles_.slaves[i];
    }
    return true;
}


bool ConfigManager::IsReady(){
    bool ret = selection_type_.Load() == kPerfect;
    if (!ret){
        LOG(WARNING)<<"ConfigManager::IsReady()  == false  "<<selection_type_;
    }
    return ret;
}


/************************************************************************/
/* courier api                                                          */
/************************************************************************/
void ConfigManager::GetRolesHandler(int err_code, Roles& roles){
    slave_channel_set_    = false;
    roles_ = roles;        // roles changed
    
    if (roles.master.compare(home_host_port_) == 0){
        is_master_ = true;
    }else{
        is_master_ = false;
    }
    LOG(WARNING)<<"ConfigManager::GetRolesHandler() I am master=(0/1)"<<is_master_;    
    // 如果当前节点不是协调者，但是master， 需要创建slave 的channel
    if (IsCoordinator_ != true && is_master_){
        LOG(WARNING)<<"ConfigManager::GetRolesHandler IsCoordinator_="<<IsCoordinator_
         <<" roles.master="<<roles.master<<" home_host_port="<<home_host_port_;
        if ((int)(roles_.slaves.size() + 1) < replica_size_){
            LOG(WARNING)
             <<" ConfigManager::GetRolesHandler() slave not enough!!! system can server for read";
            selection_type_ = kDone;
        }else{                                        
            selection_type_ = kPerfect;
            LOG(WARNING)<<"ConfigManager::GetRolesHandler()  OK!!! system election finished";  // todo, 
        }
        ClearChannels();
        CreateChannels();
    }
    
    if (is_master_){
        slave_channels_.clear();
        bool ret = AssignSlaveChannel();
        if (ret){
            slave_channel_set_    = true;
            LOG(WARNING)<<"ConfigManager::GetRolesHandler() AssignSlaveChannel() OK!";
        }
    }
}

bool ConfigManager::IsMaster(){
    return is_master_;
}

void ConfigManager::GotLeaderShipHandler(int err_code, bool got_leadership){
    if (err_code==0 && got_leadership == true){
        IsCoordinator_ = true;        // 失去资格后怎么设置;   todo 需要开始一次角色分配
        LOG(WARNING) <<"ConfigManager::GotLeaderShipHandler() I am the coordinator(leader)!!!!!, host_port="
         <<home_host_port_;
        ldd::util::MutexLock l(&mutex_);
        while(selection_type_ == kOnSelecting){
            LOG(WARNING)<<"ConfigManager::GotLeaderShipHandler() a selection is in processing, wait()";
            cond_.Wait();
        }

        StartAssignRoles();
    }else{
        LOG(ERROR)<<"ConfigManager::GotLeaderShipHandler() error_code="<<err_code
                   << " got_leadership="<<got_leadership;
    }
}

void ConfigManager::AliveNodeChangedHandler(int err_code, NodeChangedType type, 
                                            const std::string& host_port){
    ldd::util::MutexLock l(&mutex_);
    while (selection_type_ == kOnSelecting){
        LOG(WARNING)<<"ConfigManager::AliveNodeChangedHandler() a selection is in processing, wait()";
        cond_.Wait();                 
    }

    DLOG(WARNING)<<"ConfigManager::AliveNodeChangedHandler(), type="
        <<type<<" host_port="<<host_port;
    if (type == kCreateNode){
        for (size_t i=0; i<alive_node_vec_.size(); i++){
            if (host_port.compare(alive_node_vec_[i]) == 0){
                LOG(WARNING)<<"already exist host_port:"<<host_port;
                return;
            }
        }
        alive_node_vec_.push_back(host_port);
        LOG(WARNING)<<"ConfigManager::AliveNodeChangedHandler()add new host_port="<<host_port;
    }else if (type == kDeleteNode){
        int hit = -1;
        for (size_t i=0; i<alive_node_vec_.size(); i++){
            LOG(WARNING)<<"ConfigManager::AliveNodeChangedHandler()compare host_port="<<host_port
                <<" server_list="<<alive_node_vec_[i];
            if (host_port.compare(alive_node_vec_[i]) == 0){
                hit = static_cast<int>(i);
                DLOG(WARNING)<<"ConfigManager::AliveNodeChangedHandler()hit host_port="<<host_port
                    <<" server_list="<<alive_node_vec_[i];
                break;
            }
        }
        if (hit == -1){
            LOG(WARNING)<<"ConfigManager::AliveNodeChangedHandler()not exist host_port:"<<host_port;
            return;
        }
        alive_node_vec_.erase(alive_node_vec_.begin() + hit);
        std::map<std::string,  ConfigManager::Notification>::iterator itr ;
        itr = alive_node_map_.find(host_port);
        if (itr != alive_node_map_.end()){
            ConfigManager::Notification& n = itr->second;
            n.channel->Close();
            n.channel.reset();
            alive_node_map_.erase(itr);
            LOG(WARNING)<<"ConfigManager::AlineNodeChangedHandler() close channel, host_port="<<host_port;
        }
        LOG(WARNING)<<"ConfigManager::AlineNodeChangedHandler() \
                   delete computer node, host_port="
                             <<host_port<<" vec.size="<<alive_node_vec_.size()
                             <<" map.size="<<alive_node_map_.size();    
    }

    if (IsCoordinator_){
        LOG(WARNING)<<"ConfigManager::AlineNodeChangedHandler() StartAssignRoles()!!!!!!!!!!";
        StartAssignRoles();
    }
}

int ConfigManager::SetUpCourier(Options& option, std::string& courier_host_port, 
                                std::string& courier_root_path){
    int ret = 0;
    courier_host_port_ = courier_host_port;
    courier_root_path_ = courier_root_path;
    ret = Keeper::Init(option, courier_host_port_, 
                        home_host_port_, courier_root_path_, &keeper_);
    if (ret != 0){
        LOG(ERROR)<<"courier init error, ret="<<ret;
        return ret;
    }

    ret = keeper_->GetReplicaSize(&replica_size_);
    if (ret != 0 ){
        LOG(ERROR)<<"courier init error, ret="<<ret;
        return ret;
    }

    ret = keeper_->GetStoreEngine(&store_engine_);
    if (ret != 0 ){
        LOG(ERROR)<<"courier init error, ret="<<ret;
        return ret;
    }
    ret = keeper_->GetMaxBinLogFile(&binlog_size_);
    if (ret != 0 ){
        LOG(ERROR)<<"courier init error, ret="<<ret;
        return ret;
    }
    ret = keeper_->GetBucketSize(&bucket_size_);
    if (ret != 0 ){
        LOG(ERROR)<<"courier init error, ret="<<ret;
        return ret;
    }

    LOG(WARNING)<<"ConfigManager::SetUpCourier() replica_size="<<replica_size_
                 <<" store_engine="<<store_engine_<<" binlog_size="
                 <<binlog_size_<<" bucket_size="<<bucket_size_;
    
    ret = keeper_->GetRoles(boost::bind(&ConfigManager::GetRolesHandler, 
                                this, _1, _2), &roles_);
    if (ret != 0 ){
        LOG(ERROR)<<"courier init GetRoles, ret="<<ret;
        return ret;
    }
    LOG(WARNING)<<"ConfigManager::SetUpCourier(), GetRoles()"
                <<" master="<<roles_.master;

    LOG(WARNING)<<"ConfigManager::SetUpCourier(), StartLeaderElection()";
    ret = keeper_->StartLeaderElection(
              boost::bind(&ConfigManager::GotLeaderShipHandler, this, _1, _2));
    if (ret != 0 ){
        LOG(ERROR)<<"courier StartLeaderElection error, ret="<<ret;
        return ret;
    }

    ret = keeper_->GetAliveNodes(boost::bind(
                    &ConfigManager::AliveNodeChangedHandler, this, _1, _2, _3), 
                    &alive_node_vec_);
    if (ret != 0 ){
        LOG(ERROR)<<"courier GetAliveNodes error, ret="<<ret;
        return ret;
    }
    LOG(WARNING)<<"ConfigManager::SetUpCourier(), GetAliveNodes(), node size="
                <<alive_node_vec_.size();

    return 0;
}


}//namespace storage
}//namespace ldd

