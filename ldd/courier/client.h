#ifndef __LDD_COURIER_CLIENT_H_
#define __LDD_COURIER_CLIENT_H_

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


class Client{
public:
	Client();
	virtual ~Client();

public:
	static  int Init(const Options& option, const std::string& host_port, const std::string& root_path,
					 Client** broker_ptr); 
	
	virtual int GetProxyList(NodeListChangedNotify cb, 
							std::vector<std::string>* server_list)  = 0;        // /proxy_nodes
private:
	Client(const Client&);
	const Client& operator=(const Client&);

};


}//namespace courier
}//namespace ldd



#endif











