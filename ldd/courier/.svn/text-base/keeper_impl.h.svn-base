#ifndef __LDD_COURIER_KEEPER_IMPL_H_
#define __LDD_COURIER_KEEPER_IMPL_H_


#include "keeper.h"

namespace ldd{
namespace courier{


class ZkBrokerStore;
class KeeperImpl : public Keeper{
public:
	KeeperImpl();
	virtual ~KeeperImpl();

public:
	int Init(const Options& option,const std::string& server_host_port, 
					 const std::string& home_host_port,
					 const std::string& root_path, KeeperImpl* b); 
	// base conf
	virtual int GetReplicaSize(int *size)  ;
	virtual int GetStoreEngine(std::string* engine_name)  ;
	virtual int GetMaxBinLogFile(int *size)  ;
	virtual int GetBucketSize(int *size);

	virtual int StartLeaderElection(LeaderShipChangedNotify cb);
	virtual std::set<uint16_t>&	GetBucketSet();

	virtual int AddNewRoles(const Roles& roles);
	virtual int GetRoles(RolesChangedNotify cb, Roles* roles);
	virtual int GetAliveNodes(OnlineNodesChangedNotify cb, std::vector<std::string>* server_list);

public:
	void	SetZooKeeperBroker(ZkBrokerStore* z);

private:
	KeeperImpl(const KeeperImpl&);
	void operator=(const KeeperImpl&);

private:
	ZkBrokerStore* zk_broker_;

};


	}//namespace courier
}//namespace ldd



#endif








