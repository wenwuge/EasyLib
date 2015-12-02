#ifndef __LDD_CLIENT_RAW_PROVIDER__H_
#define __LDD_CLIENT_RAW_PROVIDER__H_

#include <vector>
#include <map>
#include <string>
#include <tr1/functional>
#include <ldd/util/mutex.h>
#include "ldd/courier/client.h"


namespace ldd {
namespace client {
namespace raw {

typedef std::vector<std::pair<std::string, int> > ProxyHosts;

class Provider {
public:
    Provider() : id_(0) {}
    virtual ~Provider() {

	}

	int RegisterNotify(courier::NodeListChangedNotify	notify);
    void UnRegisterNotify(int id);
    void NodeListChangedNotify(int err_code, courier::NodeChangedType type, 
                    const std::string& node);

    virtual bool Init() = 0;
    ProxyHosts& hosts();

protected:
    ProxyHosts hosts_;
    std::map<int, courier::NodeListChangedNotify> v_notify_;
    util::Mutex mutex_;
    int id_;
};

class ProxyStaticProvider : public Provider {
public:
    ProxyStaticProvider(const char* hosts);
    virtual ~ProxyStaticProvider() {
	}

    virtual bool Init();
    const char* hostname_;
};


typedef boost::function<void(int err_code)>HandleZKERROR;
class ProxyZkProvider : public Provider {
public:
    //ProxyZkProvider(std::string zk_host, std::string zk_path, int zk_timeout, HandleZKERROR cb);
	ProxyZkProvider(std::string zk_host, std::string zk_path, int zk_timeout);
    virtual ~ProxyZkProvider();

    virtual bool Init();

    void HandleError(int error_code);

private:
	courier::Client* zk_client_;
    courier::Options option_;
	
private:
	int ret_;
	std::vector<std::string> server_list_; // host_port
};


} // namespace ldd {
} // namespace client {
} // namespace raw {


#endif // __LDD_CLIENT_RAW_PROVIDER__H_





