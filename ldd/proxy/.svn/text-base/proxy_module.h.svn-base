#ifndef __LDD__STORAGE_PROXY_MODULE_H_
#define __LDD__STORAGE_PROXY_MODULE_H_


#include <string>
#include "ldd/skel/module.h"



class Proxy;
class ProxyModule : public ldd::skel::SimpleModule{
public:
	ProxyModule();
	~ProxyModule();
	virtual bool InitModule(ModuleCtx* ctx);


private:
    Proxy*  proxy_;
    int listen_port_;                   //"listen port, default: 9527"
    int thread_number_;                       //"threads count, default: 1"

    std::string zookeeper_host_;
    std::string zookeeper_rootpath_;    //"zookeeper host port list, default: \"\""
    int zookeeper_timeout_;             //"zookeeper time out, default: 1000ms

};






#endif //__LDD__STORAGE_PROXY_MODULE_H_
