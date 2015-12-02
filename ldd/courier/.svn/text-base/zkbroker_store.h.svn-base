
#ifndef __LDD_COURIER_ZK_BROKER_STORE_H_
#define __LDD_COURIER_ZK_BROKER_STORE_H_

#include <vector>
#include <string>
#include <stdint.h>
#include <map>
#include <set>
#include "keeper.h"
#include "zkbroker.h"


namespace ldd{
namespace courier{

				
class  ZkBrokerStore : public ZooKeeperBroker {
public:
	ZkBrokerStore(const std::string& home_host_port);
	virtual ~ZkBrokerStore();
	
public:
	virtual int LoadAppConfig(); 
public:

	// for interface
	int StartLeaderElection(LeaderShipChangedNotify cb);
	int DoLeaderElection();
	std::set<uint16_t>&	 GetBucketSet();
	int AddNewRoles(const Roles& roles);
	int GetRoles(RolesChangedNotify cb, Roles* roles);
	int GetAliveNodes(OnlineNodesChangedNotify cb, std::vector<std::string>* server_list);


	// 
	void RolesChangedCB(std::string& roles_desc);
	void CheckHomeHostPort();
	void GetFarmIdPair(const std::string& host_port, const std::string& farm_id);

	// configure info, need wait()
	int GetNodeList(RedoPolicy* rp);
	int GetFarmIdOfNode(RedoPolicy* rp,  const std::string& host_port);
	int CreateAliveNode(RedoPolicy* rp,  const string& host_port);
	int GetAliveNodeList(RedoPolicy* rp);
	int GetRolesChildrenList(RedoPolicy* rp);
	int GetRolesDesc(RedoPolicy* rp, const string& version_path);
    int DeleteRolesDesc( const string& version_path );

	// for leader ship election
	int CheckCordNodeExist(RedoPolicy* rp);
	int GetCordNodeList(RedoPolicy* rp);
	int CreateNodeUnderCord(RedoPolicy* rp);
	int CreateVersionNode(RedoPolicy* rp, string & value);


	void	CheckCordChildrenList();
	void	SetNodePrefix();
	void	LookupNode();

private:
	void	SortChildren(char ** data, int32_t count);
	string	ChildFloor();
	int 	CheckPrecedorExist();
	void    FilterOwnBucketID();

private:
	friend class CordChildrenListCB;
	friend class CreateNodeUnderCordCB;

	friend class NodesMapCB;
	friend class GetFarmIdOfNodeCb;
	friend class AliveNodeMapCB;

	friend class VersionNodeMapCB;
	friend class GetVersionValueCb;

	std::string							home_host_port_;

	// for configure
	std::map<std::string, std::string>	node_collection_; // <host0:port0, farm0>
	std::vector<std::string>			alive_node_collection_; 

	std::set<uint16_t>					bucket_set_;


	// for leader_election
	string						cord_node_path_;
	string						farm_id_;
	std::vector<std::string>	cord_collection_;
	string						node_prefix_;
	string						node_id_under_cord_;
	string						cur_leader_;
	string						precedor_name_;
	bool						isLeader_;
	LeaderShipChangedNotify		leader_ship_notify_;
	RolesChangedNotify			roles_change_notify_;		
	OnlineNodesChangedNotify	online_nodes_changed_notify_;
	Roles						cur_roles_st_;
    string                      cur_version_path_;

};

}//namespace courier
}//namespace ldd


#endif // __LDD_COURIER_ZK_BROKER_STORE_H_
