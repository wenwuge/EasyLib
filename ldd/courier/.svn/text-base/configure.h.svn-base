#ifndef __LDD_COURIER_CONFIGURE_H_
#define __LDD_COURIER_CONFIGURE_H_

#include <string>
#include <vector>
#include <tr1/functional>
#include "options.h"
#include "courier.h"


namespace ldd{
namespace courier{

//  function
typedef std::tr1::function<void(int err_code, NodeChangedType type, 
								const std::string& node)>NodeListChangedNotify;


class Configure{
public:
	Configure();
	virtual ~Configure();

public:
	static  int Init(const Options& option,
                     const std::string& home_host_port,
                     const std::string& zk_host_port, 
                     const std::string& root_path,
					 Configure** broker_ptr); 
	// base conf
	virtual int GetReplicaSize(int *size) = 0;
	virtual int GetStoreEngine(std::string* engine_name) = 0;
	virtual int GetMaxBinLogFile(int *size) = 0;
	virtual int GetBucketSize(int *size) = 0;

	virtual int GetNameSpaceID(const std::string& ns, int* id) = 0;
	virtual int FindFarmIdInBucketList(const int bucket_id, int* farm_id)  = 0;
	virtual int GetRoles(const int farm_id, Roles** roles)	 = 0;
	virtual int GetNodeList(NodeListChangedNotify cb, 
							std::vector<std::string>* server_list)  = 0;
	virtual int GetFarmList(std::vector<int>** farm_list)  = 0;

private:
	Configure(const Configure&);
	const Configure& operator=(const Configure&);

};


}//namespace courier
}//namespace ldd



#endif











