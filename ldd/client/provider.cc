#include <errno.h>
#include <string>
#include <string.h>
#include <vector>
#include <glog/logging.h>
#include <sys/time.h>
#include <algorithm>

#include "provider.h"
#include "internal/str_util.h"
#include <boost/bind.hpp>

namespace ldd {
namespace client {
namespace raw {

using namespace std;


ProxyHosts& Provider::hosts() {
    return hosts_;
}

int Provider::RegisterNotify(courier::NodeListChangedNotify notify){
    util::MutexLock l(&mutex_);
    id_++;
    v_notify_.insert(std::make_pair<int, courier::NodeListChangedNotify>(id_, notify));
    LOG(INFO)<<"Provider::RegisterNotify() register id="<<id_;
	return id_;   
}

void Provider::UnRegisterNotify(int id){
    util::MutexLock l(&mutex_);
    std::map<int, courier::NodeListChangedNotify>::iterator itr 
        = v_notify_.find(id);
    if (itr != v_notify_.end()){
        v_notify_.erase(itr);
         LOG(INFO)<<"Provider::UnRegisterNotify() unregister id="<<id;
    }
    

   /* for (; itr != v_notify_.end();){
        if (itr->first == id){
            v_notify_.erase(itr);
             LOG(INFO)<<"Provider::UnRegisterNotify() unregister id="<<id;
        }else{
            itr++;
        }
    }*/
    LOG(INFO)<<"Provider::UnRegisterNotify()";
    return;   
}

void Provider::NodeListChangedNotify(int err_code, 
                            courier::NodeChangedType type, 
                            const std::string& node){
    util::MutexLock l(&mutex_);
    std::map<int , courier::NodeListChangedNotify>::const_iterator itr = v_notify_.begin();
    for (; itr != v_notify_.end(); itr++){
        (itr->second)(err_code, type, node);
        LOG(INFO)<<"Provider::NodeListChangedNotify() notify id="<<itr->first
                        <<"code="<<err_code
                        <<" type="<<type<<" node="<<node;
    }
}



ProxyStaticProvider::ProxyStaticProvider(const char* hostname):
        hostname_(hostname){
}

bool ProxyStaticProvider::Init(){
    char* host_port = NULL;
    char *hosts = strdup(hostname_);
    host_port=strtok(hosts, ",");
    while ( host_port){     
        std::string host;
        int port = 0;

        std::string temp = host_port;
        bool ret = GetHostPost(temp, host, port);
        if (!ret){
            LOG(ERROR)<<"ProxyStaticProvider::Init() host_port error";
            continue;
        }
        LOG(INFO)<<"ProxyStaticProvider::Init() add host="<<host<<" port="<<port;
        hosts_.push_back(make_pair(host, port));

        host_port = strtok(NULL, "," );
    } 
    if (hosts_.empty()){
        LOG(ERROR)<<"ProxyStaticProvider::Init() error no host port available";
        return false;
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand((unsigned)tv.tv_usec);
    random_shuffle(hosts_.begin(), hosts_.end());
    LOG(INFO)<<"ProxyZkProvider::Init() srand(num)="<<tv.tv_usec;
	free(hosts);
	return true;
}


//ProxyZkProvider::ProxyZkProvider(std::string zk_hosts, std::string zk_path, 
//                                 int zk_timeout, HandleZKERROR cb){
ProxyZkProvider::ProxyZkProvider(std::string zk_host, std::string zk_path, 
								 int zk_timeout){
	option_.timeout = zk_timeout;
    /*if (cb.empty()){
        option_.cb = cb;
    }else{
        option_.cb = boost::bind(&ProxyZkProvider::HandleError, this, _1);
    }*/
    
    option_.cb = boost::bind(&ProxyZkProvider::HandleError, this, _1);
	ret_ = courier::Client::Init(option_, zk_host, zk_path, &zk_client_);
}	// todo replace by self defined function

void ProxyZkProvider::HandleError(int error_code){
    LOG(ERROR)<<"ProxyZkProvider::HandleError(), exit()";
    exit(1);
}

ProxyZkProvider::~ProxyZkProvider(){
}

bool ProxyZkProvider::Init(){
	if (ret_ != 0 ){
		LOG(ERROR)<<"ProxyZkProvider::Init() courier init error";
		return false;
	}
	
    int ret = zk_client_->GetProxyList(boost::bind(&Provider::NodeListChangedNotify, 
                                        this, _1, _2, _3), 
                                        &server_list_);
	if (ret != 0){
		LOG(ERROR)<<"ProxyZkProvider::Init() ProxyZkProvider initialize erro";
		return false;
	}
	
	std::string host; int port = 0;
    std::vector<std::string>::const_iterator itr = server_list_.begin();
    for (; itr != server_list_.end(); itr++)
    {
        bool ret = GetHostPost(*itr, host, port);
        if (!ret){
            LOG(ERROR)<<"ProxyZkProvider::Init() host_port error";
            continue;
        }
        LOG(INFO)<<"ProxyZkProvider::Init() add host="<<host<<" port="<<port;
        hosts_.push_back(make_pair(host, port));
    }
    
	/*for (size_t i=0; i<server_list_.size(); i++){
		bool ret = GetHostPost(server_list_[i], host, port);
		if (!ret){
			LOG(ERROR)<<"ProxyZkProvider::Init() host_port error";
			continue;
		}
		LOG(INFO)<<"ProxyZkProvider::Init() add host="<<host<<" port="<<port;
		hosts_.push_back(make_pair(host, port));
	}*/
	if (hosts_.empty()){
		LOG(ERROR)<<"ProxyZkProvider::Init() error no host port available";
		return false;
	}

    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand((unsigned)tv.tv_usec);
    random_shuffle(hosts_.begin(), hosts_.end());
	
    LOG(INFO)<<"ProxyZkProvider::Init() srand(num)="<<tv.tv_usec;
	return true;
}


} // namespace ldd {
} // namespace client {
} // namespace raw {

