#ifndef __LDD_COURIER_BROKER_IMPL_H_
#define __LDD_COURIER_BROKER_IMPL_H_


#include "configure.h"

namespace ldd{
namespace courier{


class ZkBrokerProxy;
class ConfigureImpl : public Configure{
public:
	ConfigureImpl();
	virtual ~ConfigureImpl();

public:
	int Init(const Options& option, 
            const std::string& home_host_port,
            const std::string& zk_host_port, 
            const std::string& root_path, 
			ConfigureImpl* b); 
	// base conf
	virtual int GetReplicaSize(int *size);
	virtual int GetStoreEngine(std::string* engine_name);
	virtual int GetMaxBinLogFile(int *size);
	virtual int GetBucketSize(int *size);

	virtual int GetNameSpaceID(const std::string& ns, int* id);
	virtual int FindFarmIdInBucketList(const int bucket_id, int* farm_id);
	virtual int GetRoles(const int farm_id, Roles** roles);
	virtual int GetNodeList(NodeListChangedNotify cb, 
							std::vector<std::string>* server_list);
	virtual int GetFarmList(std::vector<int>** farm_list)  ;

	void	SetZooKeeperBroker(ZkBrokerProxy* z);

private:
	ConfigureImpl(const ConfigureImpl&);
	void operator=(const ConfigureImpl&);

private:
	ZkBrokerProxy * zk_proxy_broker_;

};


}//namespace courier
}//namespace ldd



#endif








