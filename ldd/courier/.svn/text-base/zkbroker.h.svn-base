
#ifndef _LDD_COURIER_ZK_BROKER_H_
#define _LDD_COURIER_ZK_BROKER_H_

#include <string>
#include <map>
#include <ldd/util/atomic.h>
#include <ldd/util/mutex.h>
#include "options.h"
#include "courier.h"

namespace ldd{
namespace courier{

using namespace std;


class ZooKeeperReactor;
class ZkContext;
class CompletionCallBack;

extern const char* BASE_CONF_PATH		;
extern const char* REPLICA_NODE_PATH	;
extern const char* ENGINE_NODE_PATH		;
extern const char* BIN_LOG_PATH			;
extern const char* BUCKET_NODE_PATH		;

extern const char* NODE_LIST_PATH		;
extern const char* NAME_SPACE_LIST_PATH ;
extern const char* BUCKET_LIST_PATH		;
extern const char* FARM_LIST_PATH		;
extern const char* COORDINATION			;
extern const char* ROLES_PATH			;
extern const char* VERSION_PATH			;
extern const char* ALIVE_NODES_LIST_PATH;		

extern const char* PROXY_NODE_LIST_PATH ;

extern const int HISTORY_ROLES_NUMBER;

class RedoPolicy;
class  ZooKeeperBroker {
public:
    ZooKeeperBroker();
	virtual ~ZooKeeperBroker();
	
public :
	// derive from class Broker;
	int GetReplicaSize(int *size)  ;
	int GetStoreEngine(std::string* engine_name)  ;
	int GetMaxBinLogFile(int *size) ;
	int GetBucketSize(int *size)  ;
	int GetBucketIDList(RedoPolicy* rp);
	int GetFarmIdOfBucketID(RedoPolicy* rp, std::string& bucketID);  // call by GetFarmList completion call back

public:
	int				LoadConf();
	int				RunFromConfig(const Options& option, const std::string& host_port, 
							const std::string& root_path);
	virtual int		LoadAppConfig();
    virtual int     LoadBaseConfig();

	// get / set
	RedoPolicy*			CurRedoPolicy();
	ZooKeeperReactor*	CurReactor();
	void				SetCourierErrorCode(CourierError e );
	int					CourierErrorCode();

	std::string&	GetRootPath();
    util::RWMutex*			GetRwLockPtr();

	void			SortRolesVersionChildren(char ** data, int32_t count) ;
	bool			GetRolesFromJson(const std::string& role_desc, Roles* roles);

	void			WaitAll();
public:
	void			IncrItemNum();
	void			DecrItemNum();

public:
	// get info from "confs/"
	int		_GetReplicaSize(RedoPolicy*rp);
	int		_GetStoreEngine(RedoPolicy*rp);
	int		_GetMaxBinLogFile(RedoPolicy*rp);
	int		_GetBucketSize(RedoPolicy*rp);
protected:
	map<string, string>			bucket_collection_; // <bucketid--farmid>
    map<int, int>			    bucket_farmid_collection_; // <bucketid--farmid>

private:
	friend	class NodeListCB;
	friend	class BucketListCB;
	friend	class ReplicaNodeCB;
	friend	class EngineNodeCB;
	friend	class GetMaxBinLogCB;
	friend	class GetBucketSizeCB;
	friend  class GetFarmIdOfBucketIdCB;
	friend	class FarmListCB;
    HandleZkError               handle_error_;

	ZooKeeperReactor*			reactor_;

    util::RWMutex				rwmtx_;
    util::Mutex					mu_;
    util::CondVar				cv_;
	int							load_item_num_;

	std::string					root_path_;

	int							replica_size_;
	std::string					engine_name_;
	int							binlog_max_;
	int							buckets_size_;

	RedoPolicy*					redo_policy_; 
	bool						user_define_redo_policy_;

    util::Atomic<int>			cur_error_code_;

private:
	ZooKeeperBroker(const ZooKeeperBroker&);
	const ZooKeeperBroker& operator=(const ZooKeeperBroker&);

};



}//namespace courier
}//namespace ldd


#endif // _LDD_COURIER_ZK_BROKER_H_
