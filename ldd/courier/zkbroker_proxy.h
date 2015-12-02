
#ifndef __LDD_COURIER_ZK_BROKER_PROXY_H_
#define __LDD_COURIER_ZK_BROKER_PROXY_H_

#include <string>
#include <map>
#include "zkbroker.h"
#include "configure.h"

namespace ldd{
namespace courier{


class ZooKeeperReactor;
class ZkContext;
class CompletionCallBack;


class  ZkBrokerProxy : public ZooKeeperBroker{
public:
    ZkBrokerProxy(const std::string& home_host_port);
	virtual ~ZkBrokerProxy();
	
public:

	int GetNameSpaceID(const std::string& ns, int* id) ;
	int FindFarmIdInBucketList(const int bucket_id, int* farm_id);
	int GetFarmList(std::vector<int>** farm_list);
	int GetRoles(const int farm_id, Roles** roles);
	int GetNodeList(NodeListChangedNotify cb, 
							std::vector<std::string>* server_list) ;

public:
	virtual int LoadAppConfig();

public:

	int GetNodeList(RedoPolicy* rp);
    int CreateProxyNode(RedoPolicy* rp);
	int GetFarmList(RedoPolicy* rp);
	int GetNsIdList(RedoPolicy* rp);
	int GetNsIdInt(RedoPolicy* rp, string& ns);

	
	int GetRolesChildrenList(RedoPolicy* rp, const string& farm_id);
	int GetRolesDesc(RedoPolicy* rp, const string& farm_id, const string& version_path);
private:
	friend	class NodeListCB;
	friend	class NSListCB;
	friend  class GetNsIdIntCB;
	friend	class BucketListCB;
	friend	class ReplicaNodeCB;
	friend	class EngineNodeCB;
	friend  class GetFarmIdOfBucketIdCB;
	friend	class FarmListCB;
	friend	class ProxyRolesChildrenMapCB;
	friend  class ProxyGetVersionValueCb;

	std::map<string, int>			ns_collection_;
	std::vector<std::string>		node_collection_;   // <host_port0, host_port1>

	//map<string, Roles>			farm_roles_map_;   // <farmid--version_roles_value>	for struct Roles;
	std::map<int, Roles>			farm_roles_map_;   // <farmid--version_roles_value>	for struct Roles;

	NodeListChangedNotify			node_list_notify_;

	std::vector<int>				farm_collection_;

private:
	ZkBrokerProxy(const ZkBrokerProxy&);
	const ZkBrokerProxy& operator=(const ZkBrokerProxy&);
    std::string home_host_port_;
};

}//namespace courier
}//namespace ldd


#endif // __LDD_COURIER_ZK_BROKER_PROXY_H_
