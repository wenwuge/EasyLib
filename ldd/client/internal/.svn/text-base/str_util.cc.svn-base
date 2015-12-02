#include <glog/logging.h>
#include "str_util.h"


#include <stdlib.h>

namespace ldd {
namespace client {
namespace raw {


bool GetHostPost(const std::string& host_port, std::string& host, int & port){
	size_t pos = host_port.find(':');
	LOG(INFO)<<"GetHostPost() host_port="<<host_port<<" pos="<<pos;
	if (pos == std::string::npos){
		LOG(ERROR)<<"GetHostPost(), error host_port:"<<host_port;
		return true;
	}
	std::string strPos(host_port.begin()+pos+1, host_port.end());
	port = atoi(strPos.data());
	if (port <= 0){
		LOG(ERROR)<<"GetHostPost(), error host_port:"<<host_port;
		return false;
	}

	host.assign(host_port.begin(), host_port.begin()+pos);
	LOG(INFO)<<"GetHostPost() host="<<host<<" port="<<port;
	return true;
}

} // namespace ldd {
} // namespace client {
} // namespace raw {


