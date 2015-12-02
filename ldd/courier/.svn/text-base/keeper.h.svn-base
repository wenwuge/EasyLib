#ifndef __LDD_COURIER_KEEPER_H_
#define __LDD_COURIER_KEEPER_H_

#include <string>
#include <vector>
#include <set>
#include <tr1/functional>
#include "options.h"
#include "courier.h"

namespace ldd{
namespace courier{


//  function
typedef std::tr1::function<void(int err_code, NodeChangedType type, 
								const std::string& host_port)>OnlineNodesChangedNotify;
typedef std::tr1::function<void(int err_code, bool got_leadership)>LeaderShipChangedNotify;
typedef std::tr1::function<void(int err_code, Roles& roles)>RolesChangedNotify;

class Keeper{
public:
	Keeper();
	virtual ~Keeper();

public:
	static  int Init(const Options& option, const std::string& zk_server_host_port, 
					 const std::string& home_host_port,
					 const std::string& root_path, 
					 Keeper** keeper_ptr);
	// base conf
	virtual int GetReplicaSize(int *size)  = 0;
	virtual int GetStoreEngine(std::string* engine_name)  = 0;
	virtual int GetMaxBinLogFile(int *size)  = 0;
	virtual int GetBucketSize(int *size)  = 0;

	virtual int StartLeaderElection(LeaderShipChangedNotify cb) = 0;

	virtual std::set<uint16_t>&	 GetBucketSet() = 0;

	virtual int GetAliveNodes(OnlineNodesChangedNotify cb, std::vector<std::string>* server_list) = 0;
	virtual int AddNewRoles(const Roles& roles) = 0;
	virtual int GetRoles(RolesChangedNotify cb, Roles* roles) = 0;

	//current active node

private:
	Keeper(const Keeper&);
	void operator=(const Keeper&);

};


}//namespace courier
}//namespace ldd



#endif








