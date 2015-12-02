
#ifndef __LDD_COURIER_ZK_BROKER_CLIENT_H_
#define __LDD_COURIER_ZK_BROKER_CLIENT_H_

#include <string>
#include <map>
#include "zkbroker.h"
#include "client.h"

namespace ldd{
namespace courier{


class ZooKeeperReactor;
class ZkContext;
class CompletionCallBack;


class  ZkBrokerClient : public ZooKeeperBroker{
public:
    ZkBrokerClient();
	virtual ~ZkBrokerClient();
	
public:
	int GetProxyList(NodeListChangedNotify cb, 
							std::vector<std::string>* server_list) ;

public:
	virtual int LoadAppConfig();
    virtual int LoadBaseConfig();

public:

	int GetProxyList(RedoPolicy* rp);	

private:
	friend	class ClientNodeListCB;


	
	std::vector<std::string>		node_collection_;
	NodeListChangedNotify			node_list_notify_;

private:
	ZkBrokerClient(const ZkBrokerClient&);
	const ZkBrokerClient& operator=(const ZkBrokerClient&);
};

}//namespace courier
}//namespace ldd


#endif // __LDD_COURIER_ZK_BROKER_CLIENT_H_
