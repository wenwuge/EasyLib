#ifndef __LDD_COURIER_CLIENT_IMPLE_H_
#define __LDD_COURIER_CLIENT_IMPLE_H_


#include "client.h"

namespace ldd{
namespace courier{


class ZkBrokerClient;
class ClientImpl : public Client{
public:
	ClientImpl();
	virtual ~ClientImpl();

public:
	int Init(const Options& option, const std::string& host_port, const std::string& root_path, 
					ClientImpl* b); 
	// base conf

	virtual int GetProxyList(NodeListChangedNotify cb, 
							std::vector<std::string>* server_list);
	void	SetZooKeeperBroker(ZkBrokerClient* z);

private:
	ClientImpl(const ClientImpl&);
	void operator=(const ClientImpl&);

private:
	ZkBrokerClient * zk_client_broker_;

};


}//namespace courier
}//namespace ldd



#endif








