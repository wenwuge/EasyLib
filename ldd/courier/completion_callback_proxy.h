#ifndef __LDD_COURIER_COMPLETION_CALLBACK_PROXY_H_
#define __LDD_COURIER_COMPLETION_CALLBACK_PROXY_H_

#include <string>
#include "completion_callback.h"


namespace ldd{
namespace courier{

class ZkBrokerProxy;

class NodeListCB : public CompletionCallBack{
public:
	NodeListCB(ZkBrokerProxy* b): broker_(b){
	};

	virtual void GetChildrenCompletion(const struct String_vector *strings);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZkBrokerProxy* broker_;
};

class CreateProxyNodeCB : public CompletionCallBack{
public:
    CreateProxyNodeCB(ZkBrokerProxy* b) : broker_(b){};
      virtual void CreateCompletion(const char* value);
      virtual void Retry(RedoPolicy* redo_policy_);
      virtual void ErrorHandle();
private:
    ZkBrokerProxy* broker_;
};

class NSListCB : public CompletionCallBack{
public:
	NSListCB(ZkBrokerProxy* b): broker_(b){
	};

	virtual void GetChildrenCompletion(const struct String_vector *strings);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZkBrokerProxy* broker_;
};
class GetNsIdIntCB : public CompletionCallBack{
public:
	GetNsIdIntCB(ZkBrokerProxy* b, std::string& ns) : 
	  broker_(b), ns_(ns){};

	  virtual void GetCompletion(const char * value, int value_len, 
		  const struct Stat* stat);
	  virtual void Retry(RedoPolicy* redo_policy_);
	  virtual void ErrorHandle();
private:
	ZkBrokerProxy* broker_;
	std::string	ns_;
};

class FarmListCB : public CompletionCallBack{
public:
	FarmListCB(ZkBrokerProxy* b): broker_(b){};
	virtual void GetChildrenCompletion(const struct String_vector *strings);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZkBrokerProxy* broker_;
};
class ProxyRolesChildrenMapCB : public CompletionCallBack{
public:
	ProxyRolesChildrenMapCB(ZkBrokerProxy* b, const std::string& farm_id): 
					broker_(b),farm_id_(farm_id){};
	virtual void GetChildrenCompletion(const struct String_vector *strings);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZkBrokerProxy* broker_;
	std::string	 farm_id_;
    std::string  cur_version_path_;
};
class ProxyGetVersionValueCb : public CompletionCallBack{
public:
	ProxyGetVersionValueCb(ZkBrokerProxy* b, 
						const std::string farm_id, const std::string version) : 
						broker_(b), farm_id_(farm_id), version_path_(version){};
	virtual void GetCompletion(const char * value, int value_len, 
		const struct Stat* stat);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZkBrokerProxy*		broker_;
	const std::string	farm_id_;
	const std::string	version_path_;
};


}//namespace courier
}//namespace ldd



#endif








