#include <glog/logging.h>

//https://code.google.com/p/rapidjson/wiki/UserGuide
#include "rapidjson/document.h"		// rapidjson's DOM-style API
#include "rapidjson/prettywriter.h"	// for stringify JSON
#include "rapidjson/filestream.h"	// wrapper of C stream for prettywriter as output

#include "reactor.h"
#include "zkbroker.h"
#include "zkcontext.h"
#include "completion_callback_impl.h"
#include "watcher_action_impl.h"
#include "redo_simple_policy.h"




namespace ldd{
namespace courier{

const char* BASE_CONF_PATH					= "/confs";
const char* REPLICA_NODE_PATH				= "/replicas";
const char* ENGINE_NODE_PATH				= "/engine";
const char* BIN_LOG_PATH					= "/binlog_max";
const char* BUCKET_NODE_PATH				= "/buckets";
const char* NODE_LIST_PATH					= "/nodes";
const char* NAME_SPACE_LIST_PATH			= "/ns";
const char* BUCKET_LIST_PATH				= "/buckets";
const char* FARM_LIST_PATH					= "/farms";
const char* COORDINATION					= "/coordinator";
const char* ROLES_PATH						= "/roles";
const char* VERSION_PATH					= "/version";
const char* ALIVE_NODES_LIST_PATH			= "/alive_nodes";

const char* PROXY_NODE_LIST_PATH            = "/proxy_nodes";

const int   HISTORY_ROLES_NUMBER            = 10;

ZooKeeperBroker::ZooKeeperBroker(): 
		reactor_(NULL),cv_(&mu_), load_item_num_(0), replica_size_(0), 
		binlog_max_(0), buckets_size_(0), redo_policy_(0),
		user_define_redo_policy_(false),cur_error_code_(0){
}

ZooKeeperBroker::~ZooKeeperBroker(){
		// close reactor_	
	if (user_define_redo_policy_ == false){
		delete redo_policy_;
	}
	
	delete reactor_;
	DLOG(INFO)<<"ZooKeeperBroker::~ZooKeeperBroker()";
}


int ZooKeeperBroker::RunFromConfig(const Options& option,const std::string& host_port, 
								   const std::string& root_path){
	LOG(INFO)<<"COURIER RunFromConfiging()";
    
	reactor_ = new ZooKeeperReactor(option.cb, host_port, option.timeout);

    handle_error_ = option.cb;

	if (option.redo_policy){
		redo_policy_ = option.redo_policy;
		user_define_redo_policy_ = true;
	}else{
		redo_policy_ = new RedoPolicySimple(3, 60);
	}
	
	root_path_	= root_path;
	bool ret = reactor_->StartUp();
	if (ret != true){
		LOG(ERROR)<<"RunFromConfig() reactor start up ERROR:";
		return kSystemError;	// ERROR handle
	}

	return LoadConf();
}
	
int ZooKeeperBroker::LoadConf(){
	int ret = 0;
	/*RedoPolicy* rp = redo_policy_->Copy();
	ret = _GetReplicaSize(rp);	
	if (ret != 0){
		LOG(ERROR)<<"ZooKeeperBroker GetReplicaSize ERROR, ret:"<<ret;
		return ret;
	}
	
	rp = redo_policy_->Copy();
	ret = _GetStoreEngine(rp);
	if (ret != 0){
		LOG(ERROR)<<"ZooKeeperBroker GetStoreEngine ERROR, ret:"<<ret;
		return ret;
	}
	rp = redo_policy_->Copy();
	ret= _GetMaxBinLogFile(rp);
	if (ret != 0){
		LOG(ERROR)<<"ZooKeeperBroker GetMaxBinLogFile ERROR, ret:"<<ret;
		return ret;
	}
	rp = redo_policy_->Copy();
	ret = _GetBucketSize(rp);
	if (ret != 0){
		LOG(ERROR)<<"ZooKeeperBroker GetBucketSize ERROR, ret:"<<ret;
		return ret;
	}

	rp = CurRedoPolicy()->Copy();
	ret = GetBucketIDList(rp);
	if (ret != 0){
		LOG(ERROR)<<"ZooKeeperBroker GetMaxBinLogFile ERROR, ret:"<<ret;
		return ret;
	}*/

    ret = LoadBaseConfig();
	ret = LoadAppConfig();
	if (ret != 0){
		LOG(ERROR)<<"ZooKeeperBroker LoadAppConfig ERROR, ret:"<<ret;
		return ret;
	}

	WaitAll();   // need a timeout

	if ( CourierErrorCode() == kCourierOk){
		DLOG(INFO)<<"LoadConf() initialize successfully!!";
	}else{
		DLOG(ERROR)<<"ERROR LoadConf() fail to initialize !! errorcode="<<CourierErrorCode();
	}

    map<string, string>::iterator itr = bucket_collection_.begin(); 
    for (; itr != bucket_collection_.end(); itr++){
        string bucket = itr->first;
        string farm   = itr->second;
        int bucket_id = 0;
        int farm_id   = 0;

        if (bucket[0]>= '0' && bucket[0]<= '9'){
            bucket_id = atoi(bucket.data());
            LOG(INFO)<< "ZooKeeperBroker::LoadConf() find bucket="<<bucket_id;
        }else{
            LOG(WARNING)<< "ZooKeeperBroker::LoadConf() error bucket="<<bucket;
            continue;
        }

        if (farm[0]>= '0' && farm[0]<= '9'){
            farm_id = atoi(farm.data());
            LOG(INFO)<< "ZooKeeperBroker::LoadConf() find farm="<<farm_id;
        }else{
             LOG(WARNING)<< "ZooKeeperBroker::LoadConf() error farm="<<farm;
            continue;
        }
        LOG(INFO)<< "ZooKeeperBroker::LoadConf() insert bucket_id="
                <<bucket_id<<" farm_id="<<farm_id;
        bucket_farmid_collection_.insert(std::make_pair(bucket_id, farm_id));
    }
	return CourierErrorCode();
}

int	ZooKeeperBroker::LoadAppConfig(){ 
	// base class default implementation
	return 0 ;
}

int ZooKeeperBroker::LoadBaseConfig(){
    int ret = 0;
    RedoPolicy* rp = redo_policy_->Copy();
    ret = _GetReplicaSize(rp);	
    if (ret != 0){
        LOG(ERROR)<<"ZooKeeperBroker GetReplicaSize ERROR, ret:"<<ret;
        return ret;
    }

    rp = redo_policy_->Copy();
    ret = _GetStoreEngine(rp);
    if (ret != 0){
        LOG(ERROR)<<"ZooKeeperBroker GetStoreEngine ERROR, ret:"<<ret;
        return ret;
    }
    rp = redo_policy_->Copy();
    ret= _GetMaxBinLogFile(rp);
    if (ret != 0){
        LOG(ERROR)<<"ZooKeeperBroker GetMaxBinLogFile ERROR, ret:"<<ret;
        return ret;
    }
    rp = redo_policy_->Copy();
    ret = _GetBucketSize(rp);
    if (ret != 0){
        LOG(ERROR)<<"ZooKeeperBroker GetBucketSize ERROR, ret:"<<ret;
        return ret;
    }

    rp = CurRedoPolicy()->Copy();
    ret = GetBucketIDList(rp);
    if (ret != 0){
        LOG(ERROR)<<"ZooKeeperBroker GetMaxBinLogFile ERROR, ret:"<<ret;
        return ret;
    }
    return 0;
}

std::string& ZooKeeperBroker::GetRootPath(){
	return root_path_;
}

util::RWMutex* ZooKeeperBroker::GetRwLockPtr(){
	return &rwmtx_;
}

RedoPolicy* ZooKeeperBroker::CurRedoPolicy(){
	return redo_policy_;
}

ZooKeeperReactor* ZooKeeperBroker::CurReactor(){
	return reactor_;
}

void ZooKeeperBroker::SetCourierErrorCode(CourierError e ){
	cur_error_code_ = static_cast<int>(e);
	LOG(ERROR)<<"SetZkErrorCode ret="<<cur_error_code_.Load();
    util::MutexLock l(&mu_); // happen error should call this
    if (load_item_num_> 0 ){
        LOG(ERROR)<<"SetZkErrorCode() signal load_item_num="<<load_item_num_;
        load_item_num_ = 0;
        cv_.Signal();
    }
    LOG(ERROR)<<"ZooKeeperBroker::SetCourierErrorCode() handle_error_ CourierError"<<e;
    if (handle_error_){
        handle_error_(e);
    }
}

int ZooKeeperBroker::CourierErrorCode(){
	return cur_error_code_.Load();
}

void ZooKeeperBroker::WaitAll(){
    util::MutexLock l(&mu_);
	while (load_item_num_ > 0){
		DLOG(INFO)<<"WaitAll() waiting initialize " <<load_item_num_<<" items";
		cv_.Wait();
	}
	DLOG(INFO)<<"WaitAll() signaled!!!";
}

void ZooKeeperBroker::IncrItemNum(){
    util::MutexLock l(&mu_);
	load_item_num_++;
}

void ZooKeeperBroker::DecrItemNum(){
    util::MutexLock l(&mu_);
	load_item_num_--;
	if (load_item_num_<= 0 ){
		cv_.Signal();
	}
}

/************************************************************************/
/*pure virtual interface implementation									*/
/************************************************************************/
int ZooKeeperBroker::GetReplicaSize(int *size) { 
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ZooKeeperBroker::GetReplicaSize() ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	if (!size){
		LOG(ERROR)<<"ERROR: broker replica size invalid argument from client";
		return kInvalidArgument;
	}
    util::RWMutexLock rlock(&rwmtx_);
	*size = replica_size_;
	return 0;
}

int ZooKeeperBroker::GetStoreEngine(std::string* engine_name) {
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ZooKeeperBroker::GetStoreEngine ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	if (!engine_name){
		LOG(ERROR)<<"ERROR broker :: store engine invalid argument from client";
		return kInvalidArgument;
	}
    util::RWMutexLock rlock(&rwmtx_);
	*engine_name = engine_name_;
	return 0;
}

int ZooKeeperBroker::GetMaxBinLogFile(int *size){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ERROR ZooKeeperBroker::GetMaxBinLogFile() ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	if (!size){
		LOG(ERROR)<<"ERROR broker:: max bin log invalid argument from client";
		return kInvalidArgument;
	}
    util::RWMutexLock rlock(&rwmtx_);
	*size	= binlog_max_;
	return 0;
}

int ZooKeeperBroker::GetBucketSize(int *size) { 
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ERROR ZooKeeperBroker::GetBucketSize() ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	if (!size){
		LOG(ERROR)<<"ERROR broker: bucket size invalid argument from client";
		return kInvalidArgument;
	}
    util::RWMutexLock rlock(&rwmtx_);
	*size	= buckets_size_;
	return 0;
}

/************************************************************************/
/*   util                                                             */
/************************************************************************/
static int vstrcmpversion(const void* str1, const void* str2) {
	const char **a = (const char**)str1;
	const char **b = (const char**) str2;
	return strcmp(*a, *b); 
} 

void ZooKeeperBroker::SortRolesVersionChildren(char ** data, int32_t count) {
	if (data && count > 0){
		qsort(data, count, sizeof(char*), &vstrcmpversion);
	}
}

bool ZooKeeperBroker::GetRolesFromJson(const std::string& role_desc, Roles* roles){
	// get the role_des
	rapidjson::Document document;
	if(document.Parse<0>(role_desc.data()).HasParseError() ){
		DLOG(INFO)<<"GetRolesFromJson(), paser ERROR, desc: "<<role_desc;
		return false;
	}
	assert(document.HasMember("master"));
	assert(document.HasMember("slave"));
	assert(document.HasMember("follower"));

	DLOG(INFO)<<"GetRolesFromJson(), role_desc: "<<role_desc;

	const rapidjson::Value& master = document["master"];
	assert(master.IsArray());
	rapidjson::SizeType i=0;
	roles->master = master[i].GetString();
	DLOG(INFO)<<"GetRolesFromJson(), master: "<<roles->master;

	const rapidjson::Value& slave = document["slave"];
	assert(slave.IsArray());
	for (rapidjson::SizeType i=0; i<slave.Size(); i++){
		if (strlen(slave[i].GetString()) > 0){
			roles->slaves.push_back(slave[i].GetString());
		}else{
			LOG(WARNING)<<"ZooKeeperBroker::GetRolesFromJson() slave NULL ";
		}
		DLOG(INFO)<<"ZooKeeperBroker::GetRolesFromJson(), slaves: "<<slave[i].GetString();
	}

	const rapidjson::Value& follower = document["follower"];
	assert(follower.IsArray());
	for (rapidjson::SizeType i=0; i<follower.Size(); i++){
		if (strlen(follower[i].GetString()) > 0){
			roles->followers.push_back(follower[i].GetString());
		}else{
			LOG(WARNING)<<"ZooKeeperBroker::GetRolesFromJson() followers NULL ";
		}
		DLOG(INFO)<<"ZooKeeperBroker::GetRolesFromJson(), followers: "<<follower[i].GetString();
	}

	return true;
}

/************************************************************************/
/* need a call back                                                     */
/************************************************************************/
int ZooKeeperBroker::_GetReplicaSize(RedoPolicy* rp){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ZooKeeperBroker::_GetReplicaSize() cur state  ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	CompletionCallBack *cb			= new ReplicaNodeCB(this);
	cb->SetRedoPolicy(rp);
	//WatcherAction*		action		= new ReplicaNodeWatcherAction(this);
	std::string node_path			= root_path_+ BASE_CONF_PATH + REPLICA_NODE_PATH;
	bool ret = reactor_->GetNodeValue(node_path, cb, NULL/*action*/, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<" SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		IncrItemNum();
		return 0;
	}
}

int ZooKeeperBroker::_GetStoreEngine(RedoPolicy* rp){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ZooKeeperBroker::_GetStoreEngine() cur state  ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	CompletionCallBack *cb			= new EngineNodeCB(this);
	cb->SetRedoPolicy(rp);
	//WatcherAction	*action			= new StoreEngineNodeWatcherAction(this);
	std::string node_path			= root_path_ + BASE_CONF_PATH + ENGINE_NODE_PATH;
	int ret = reactor_->GetNodeValue(node_path, cb, NULL/*action*/, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<" SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		IncrItemNum();
		return 0;
	}
}

int ZooKeeperBroker::_GetMaxBinLogFile(RedoPolicy* rp){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ZooKeeperBroker::_GetMaxBinLogFile() cur state  ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	CompletionCallBack *cb			= new GetMaxBinLogCB(this);
	cb->SetRedoPolicy(rp);
	WatcherAction*		action		= new BinLogNodeWatcherAction(this);
	std::string node_path			= root_path_+ BASE_CONF_PATH + BIN_LOG_PATH;
	int ret = reactor_->GetNodeValue(node_path, cb, action, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<" SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		IncrItemNum();
		return 0;
	}
}

int ZooKeeperBroker::_GetBucketSize(RedoPolicy* rp){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"ZooKeeperBroker::_GetBucketSize() cur state  ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	CompletionCallBack *cb			= new GetBucketSizeCB(this);
	cb->SetRedoPolicy(rp);
	//WatcherAction	   *action		= new BucketSizeNodeWatcherAction(this);
	std::string node_path			= root_path_ + BASE_CONF_PATH + BUCKET_NODE_PATH;
	int ret = reactor_->GetNodeValue(node_path, cb, NULL/*action*/, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<" SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		IncrItemNum();
		return 0;
	}
}

int ZooKeeperBroker::GetBucketIDList( RedoPolicy* rp ){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"current ZkBrokerProxy::GetBucketIDList() cur state  ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	CompletionCallBack *cb		= new BucketListCB(this);
	cb->SetRedoPolicy(rp);
	//WatcherAction* action		= new BucketListWatcherAction(this);
	std::string	node_path		= GetRootPath() + BUCKET_LIST_PATH; 
	bool ret = CurReactor()->GetChildrenList(node_path, cb, NULL/*action*/, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"proxy:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		IncrItemNum();
		return 0;
	}
}

int ZooKeeperBroker::GetFarmIdOfBucketID( RedoPolicy* rp, std::string& bucketID ){
	if (CourierErrorCode() != 0 ){
		LOG(ERROR)<<"current ZkBrokerProxy::GetFarmIdOfBucketID() cur state  ERROR="<<CourierErrorCode();
		return CourierErrorCode();
	}
	std::string node_path;
	node_path = GetRootPath() + BUCKET_NODE_PATH + "/" + bucketID;

	CompletionCallBack *cb			= new GetFarmIdOfBucketIdCB(this, bucketID);
	cb->SetRedoPolicy(rp);
	//WatcherAction	*action			= new FarmIdNodeWatcherAction(this, bucketID);
	bool ret = CurReactor()->GetNodeValue(node_path, cb, NULL/*action*/, rp->GetRedoTimeInterval());
	if (ret == false){
		SetCourierErrorCode(kZooKeeperNotWork);
		LOG(ERROR)<<"proxy:: SetZkErrorCode ERROR, kZkNotWork";
		return kZooKeeperNotWork; 
	}else{
		IncrItemNum();
		return 0;
	}
}
	
}//namespace courier
}//namespace ldd



	



