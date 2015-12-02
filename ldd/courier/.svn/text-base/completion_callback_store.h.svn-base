#ifndef __LDD_COURIER_COMPLETION_CALLBACK_STORE_H_
#define __LDD_COURIER_COMPLETION_CALLBACK_STORE_H_

#include <string>
#include "completion_callback.h"

namespace ldd{
namespace courier{

/************************************************************************/
/* for farm                                                              */
/************************************************************************/
class ZkBrokerStore;
class CoordinatorNodeExistsCB : public CompletionCallBack{
public:
	CoordinatorNodeExistsCB(ZkBrokerStore* b): broker_(b){
	};

	virtual void ExistsCompletion(const struct Stat *stat);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZkBrokerStore* broker_;
};

class CordChildrenListCB : public CompletionCallBack{
public:
	CordChildrenListCB(ZkBrokerStore* b): broker_(b){
	};

	virtual void GetChildrenCompletion(const struct String_vector *strings);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZkBrokerStore* broker_;
};

class CreateNodeUnderCordCB : public CompletionCallBack{
public:
	CreateNodeUnderCordCB(ZkBrokerStore* b): broker_(b){
	};

	virtual void CreateCompletion(const char* value);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZkBrokerStore* broker_;
};

class CreateVersionNodeCB : public CompletionCallBack{
public:
	CreateVersionNodeCB(ZkBrokerStore* b, std::string& roles_desc): 
						broker_(b), roles_desc_(roles_desc){};

	virtual void CreateCompletion(const char* value);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZkBrokerStore* broker_;
	std::string roles_desc_;
};

class CreateAliveNodeCB : public CompletionCallBack{
public:
	CreateAliveNodeCB(ZkBrokerStore* b, const std::string& host_port) : 
	  broker_(b), host_port_(host_port){};
	  virtual void CreateCompletion(const char* value);
	  virtual void Retry(RedoPolicy* redo_policy_);
	  virtual void ErrorHandle();
private:
	ZkBrokerStore* broker_;
	std::string host_port_;
};

class NodesMapCB : public CompletionCallBack{
public:
	NodesMapCB(ZkBrokerStore* b): broker_(b){};

	virtual void GetChildrenCompletion(const struct String_vector *strings);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZkBrokerStore* broker_;
};

class GetFarmIdOfNodeCb : public CompletionCallBack{
public:
	explicit GetFarmIdOfNodeCb(ZkBrokerStore* b, const std::string& s) : broker_(b), host_port_(s){};
	virtual void GetCompletion(const char * value, int value_len, 
		const struct Stat* stat);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZkBrokerStore* broker_;
	std::string	host_port_;
};

class AliveNodeMapCB : public CompletionCallBack{
public:
	AliveNodeMapCB(ZkBrokerStore* b): broker_(b){};
	virtual void GetChildrenCompletion(const struct String_vector *strings);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZkBrokerStore* broker_;
};


class VersionNodeMapCB : public CompletionCallBack{
public:
    VersionNodeMapCB(ZkBrokerStore* b, std::string& version_path) : 
      broker_(b), cur_version_path_(version_path){

      };
	virtual void GetChildrenCompletion(const struct String_vector *strings);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZkBrokerStore* broker_;
    std::string&  cur_version_path_;
};

class GetVersionValueCb : public CompletionCallBack{
public:
	explicit GetVersionValueCb(ZkBrokerStore* b, std::string s) : 
				broker_(b), version_path_(s){};
	virtual void GetCompletion(const char * value, int value_len, 
								const struct Stat* stat);
	virtual void Retry(RedoPolicy* redo_policy_);
	virtual void ErrorHandle();
private:
	ZkBrokerStore* broker_;
	std::string	version_path_;
};





}//namespace courier
}//namespace ldd



#endif








