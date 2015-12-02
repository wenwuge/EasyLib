#include <glog/logging.h>
#include <boost/make_shared.hpp>
#include "ldd/util/time.h"
#include "ldd/protocol/server/store_error.h"
#include "../redo_simple_policy.h"
#include "get.h"
#include "check.h"
#include "atomic_incr.h"
#include "atomic_append.h"
#include "list.h"
#include "mutate.h"
#include "client_impl.h"
#include "str_util.h"
#include <boost/bind.hpp>

namespace ldd {
namespace client {
namespace raw {

ClientImpl::ClientImpl(net::Client* client, 
                       net::EventLoop* event_loop,
					   Provider *provider, 
					   const Options &option) :
          client_(client),
          event_loop_(event_loop),
		  provider_(provider),
		  option_(option),
		  proxy_index_(0),
		  redo_policy_(NULL){

        cb_id_ = provider_->RegisterNotify(boost::bind(&ClientImpl::NodeListChangedNotify, 
                                                       this, _1, _2, _3));
        LOG(INFO)<<"ClientImpl::ClientImpl() cb_id="<<cb_id_;
}

ClientImpl::~ClientImpl(){
  if (option_.rp == NULL){
	  delete redo_policy_;
  }
  provider_->UnRegisterNotify(cb_id_);

  std::map<HostPort, boost::shared_ptr<net::Channel> > ::iterator 
	  itr = nodes_.begin();
  for (; itr != nodes_.end(); itr++){
	  boost::shared_ptr<net::Channel>& channel = itr->second;
	  channel.reset();
  }
}

bool ClientImpl::Init(){
	util::RWMutexLock wlock(&rwmtx_, true);
    ProxyHosts& hosts = provider_->hosts();
    std::vector<std::pair<std::string, int> >::const_iterator it;
    for (it = hosts.begin(); it != hosts.end(); it++) {
        boost::shared_ptr<Channel> channel = 
			client_->Create(event_loop_, it->first, it->second);  
        if (channel.get() == NULL) {
            LOG(ERROR) << "ClientImpl::Init() connect to " 
                << it->first<< ":" << it->second << " failed";
            return false;
		}else{
			LOG(INFO) << "ClientImpl::Init() connect to " 
                << it->first<< ":" << it->second << " success";
		}

		nodes_.insert(make_pair(*it, channel));	
        ordered_nodes_.push_back(*it);
    }

	LOG(INFO)<<"ClientImpl::Init() nodes.size()="<<nodes_.size()
				<<" sorted_nodes.size()="<<ordered_nodes_.size();
	
	if (option_.rp == NULL){
		redo_policy_ = new RedoPolicySimple(nodes_.size(), option_.time_out);
		LOG(INFO)<<"ClientImpl::Init() use default RedoPolicySimple()";
	}else {
		LOG(INFO)<<"ClientImpl::Init() use customized RedoPolicy()";
        redo_policy_ = option_.rp;
	}
	

	LOG(INFO)<<"ClientImpl::Init(), client retry timeout="<<option_.time_out;
    return true;
}

void ClientImpl::NodeListChangedNotify(int err_code, 
									   courier::NodeChangedType type, 
									   const std::string& host_port){
    std::string host;
	int  port = 0;
	bool ret = GetHostPost(host_port, host, port);
	LOG(INFO)<<"ClientImpl::NodeListChangedNotify() host_port="<<host_port
				<<" host="<<host<<" port="<<port;
	if (!ret){
		LOG(ERROR)<<"ClientImpl::NodeListChangedNotify() host_port error";
		return;
	}
	if (type == courier::kCreateNode){
		util::RWMutexLock wlock(&rwmtx_, true);
		boost::shared_ptr<Channel> channel = 
                client_->Create(event_loop_, host, port); 
		if (channel.get() == NULL) {
			LOG(ERROR) << "ClientImpl::NodeListChangedNotify() connect to " 
								<< host<< ":" << port << " failed";
			return ;
		}
		nodes_.insert(make_pair(make_pair(host, port), channel));
		ordered_nodes_.push_back(make_pair(host, port));
		LOG(INFO)<<"ClientImpl::NodeListChangedNotify() add "<<" host="
            <<host<<" port="<<port;

	}else if (type == courier::kDeleteNode){
		 util::RWMutexLock wlock(&rwmtx_, true);
		 std::map<HostPort, boost::shared_ptr<net::Channel> >::iterator itr;
		 itr = nodes_.find(make_pair(host, port));
		 if (itr != nodes_.end()){
			 boost::shared_ptr<net::Channel>& n = (itr->second);
			 n->Close();
			 n.reset();
			 nodes_.erase(itr);
			 LOG(INFO)<<"ClientImpl::NodeListChangedNotify() after delete"
                 <<" nodes_.size="<<nodes_.size();
		 }
		 std::vector<HostPort>::iterator itr2 = ordered_nodes_.begin();
		 for (; itr2 != ordered_nodes_.end(); itr2++){
			std::pair<std::string, int> p = *itr2;
			if (host.compare(p.first)==0 && p.second == port){
				ordered_nodes_.erase(itr2);
				LOG(INFO)<<"ClientImpl::NodeListChangedNotify() delete host="
                    <<p.first<<" port="<<p.second;
				break;
			} // if
		 }// for
	}

	return;
}

bool ClientImpl::GetChannel(const std::string& filter_host, const int filter_port,
							std::string& using_host, int& using_port,
							boost::shared_ptr<net::Channel>& channel){

	util::RWMutexLock rlock(&rwmtx_);
	std::map<HostPort, boost::shared_ptr<Channel> >::iterator itr;
	if (ordered_nodes_.empty()){
		LOG(ERROR)<<"ClientImpl::GetChannel() no host port in provider";
		return false;
	}

	size_t visit_num = 0;
	do{
		visit_num++;
		//轮巡负载均衡算法
		proxy_index_ = (proxy_index_ + 1) % ordered_nodes_.size();
		HostPort& host_port_pair = ordered_nodes_[proxy_index_];
		LOG(INFO)<<"ClientImpl::GetChannel() proxy_index_="<<proxy_index_;

		itr = nodes_.find(host_port_pair);
		if (itr == nodes_.end()) {
			LOG(ERROR)<<" ClientImpl::GetChannel() error data in ordered_nodes_ and nodes_ not existent";
			return false;		
		}
		

		std::string& host = host_port_pair.first;
		int			 port = host_port_pair.second;

		if (host.compare(filter_host) == 0 && port == filter_port){
			LOG(INFO)<<"ClientImpl::GetChannel() filter :"<<host<<" port="<<port;
			continue;
		}else{
			using_host = host;
			channel = itr->second;
			LOG(INFO)<<"ClientImpl::GetChannel() using_host:"<<using_host<<" port="<<port;
			return true;
		}
	}while (visit_num < ordered_nodes_.size());

	LOG(ERROR)<<" ClientImpl::GetChannel() error go over address list, no valid, visit_num"<<visit_num;
	return false;
}


/************************************************************************/
/* impletation interfaces                                               */
/************************************************************************/
// get
int ClientImpl::Get(const std::string&ns,  const std::string& key, 
					 const DataCompletion& completion){

	RedoPolicy* rp = redo_policy_->Copy();
	rp->FireTime(util::Time::CurrentMilliseconds());
	return Get(ns, key, completion, "", 0, rp);
}

int ClientImpl::Get(const std::string&ns, const std::string& key, 
					 const DataCompletion& completion, 
					 const std::string& filter_host, int filter_port, RedoPolicy* rp){
	LOG(INFO)<<"ClientImpl::Get(), start ns="<<ns<<" key="<<key<<" filter_host="<<filter_host;

	boost::shared_ptr<Channel> channel;
	std::string using_host;
	int			using_port;
	bool ret = GetChannel(filter_host, filter_port, using_host, using_port, channel);
	if (ret){
        boost::shared_ptr<GetRequest> get = 
            boost::make_shared<GetRequest>(this, ns, key, 
                    completion, using_host, using_port, rp);

		//GetRequest* get = new GetRequest(this, ns, key, completion, using_host, using_port, rp);
        channel->Post(get, ldd::util::TimeDiff::Milliseconds(500), true);

		return ldd::protocol::kOk;
	}else{
		delete rp;
        LOG(ERROR)<<"ClientImpl::Get() GetChannel error";
		return ldd::protocol::kNoConnection; 
	}
}


//check
int ClientImpl::Check(const std::string& ns,const std::string& key, 
					   const CheckCompletion& completion) {

	RedoPolicy* rp = redo_policy_->Copy();
	rp->FireTime(util::Time::CurrentMilliseconds());
	return Check(ns, key, completion, "", 0, rp);
}

int ClientImpl::Check(const std::string& ns,const std::string& key, 
					   const CheckCompletion& completion, 
					   const std::string& filter_host, int filter_port, RedoPolicy* rp) {
	boost::shared_ptr<Channel> channel;
	LOG(INFO)<<"ClientImpl::Check(), start  ns="<<ns<<" key="<<key<<" filter_host="<<filter_host;

	std::string using_host;
	int			using_port;
	bool ret = GetChannel(filter_host, filter_port, using_host, using_port, channel);
	if (ret){
	  // CheckRequest* cr = new CheckRequest(this, ns, key, completion, using_host, using_port, rp);

       boost::shared_ptr<CheckRequest> cr = 
           boost::make_shared<CheckRequest>(this, ns, key, completion,
                                        using_host, using_port, rp);

	   channel->Post(cr,ldd::util::TimeDiff::Milliseconds(500), true);
	   return ldd::protocol::kOk;
	}else{
		delete rp;
        LOG(ERROR)<<"ClientImpl::Check() GetChannel error";
		return ldd::protocol::kNoConnection;
	}
}

//list
int ClientImpl::List(const std::string& ns, const std::string& key,
					  uint32_t u32limit_, uint8_t	u8position_,  
					  const DataCompletion& completion){
	RedoPolicy* rp = redo_policy_->Copy();
	rp->FireTime(util::Time::CurrentMilliseconds());
	return List(ns, key,u32limit_, u8position_, completion, "", 0,  rp);
}

int ClientImpl::List(const std::string& ns, const std::string& key,
					  uint32_t u32limit_, uint8_t	u8position_,  
					  const DataCompletion& completion, 
                      const std::string& filter_host, int filter_port, 
					  RedoPolicy* rp){
	LOG(INFO)<<"ClientImpl::List(), start ns="<<ns<<" key="<<key
                <<" filter_host="<<filter_host
				<< "limit="<<u32limit_<<" position="<<u8position_;

	boost::shared_ptr<Channel> channel;
	std::string using_host;
	int			using_port;
	bool ret = GetChannel(filter_host, filter_port, using_host, using_port, channel);
	if (ret){
		/*ListRequest* lr	 = new ListRequest(this, ns, key, 
										u32limit_, u8position_, 
										completion, using_host, using_port, rp);*/

        boost::shared_ptr<ListRequest> lr = 
            boost::make_shared<ListRequest>(this, ns, key, 
                                u32limit_, u8position_, 
                                completion, using_host, using_port, rp);


		channel->Post(lr,ldd::util::TimeDiff::Milliseconds(500), true);
		return ldd::protocol::kOk;
	}else{
		delete rp;
        LOG(ERROR)<<"ClientImpl::List() GetChannel error";
		return ldd::protocol::kNoConnection ;
	}
}

int ClientImpl::Mutate(const Mutation& mutation, const MutateCompletion& completion){
	RedoPolicy* rp = redo_policy_->Copy();
	rp->FireTime(util::Time::CurrentMilliseconds());
	return Mutate(mutation, completion, "", 0,  rp);
}

int ClientImpl::Mutate(const Mutation& mutation, const MutateCompletion& completion, 
						 const std::string& filter_host, int filter_port, RedoPolicy* rp){
	LOG(INFO)<<"ClientImpl::Mutate(), start filter_host="<<filter_host;

	boost::shared_ptr<Channel> channel;
	std::string using_host;
	int			using_port;
	bool ret = GetChannel(filter_host, filter_port, using_host, using_port, channel);
	if (ret){
	   /*MutateRequest* mr	 = new MutateRequest(this, mutation, 
								   completion, using_host, using_port, rp);*/

       boost::shared_ptr<MutateRequest> mr = 
           boost::make_shared<MutateRequest>(this, mutation, 
                                completion, using_host, using_port, rp);

	   channel->Post(mr,ldd::util::TimeDiff::Milliseconds(500), true);
	   return ldd::protocol::kOk;
	}else{
	   delete rp;
       LOG(ERROR)<<"ClientImpl::Mutate() GetChannel error";
	   return ldd::protocol::kNoConnection ;
	}
}

// put
int ClientImpl::Put(const std::string& ns, const std::string& key, 
					 const std::string& value, 
					 const MutateCompletion& completion){
	Mutation  mutation(ns, key, value, 0);
	return Mutate(mutation, completion);
}

//delete
int ClientImpl::Delete(const std::string& ns, const std::string& key, 
					const MutateCompletion& completion){
	Mutation  mutation(ns, key);
	return Mutate(mutation, completion);
}


//incr
int ClientImpl::Incr(const std::string& ns,const std::string& key,
					  int32_t s32operand_, int32_t	s32initial_,
					  uint64_t u64ttl_, const CasCompletion& completion){

	RedoPolicy* rp = redo_policy_->Copy();
	rp->FireTime(util::Time::CurrentMilliseconds());
	return Incr(ns, key, s32operand_, s32initial_, 
					u64ttl_, completion, "",  0, rp);	
}

int ClientImpl::Incr(const std::string& ns,const std::string& key,
					  int32_t s32operand_, int32_t	s32initial_,
					  uint64_t u64ttl_, const CasCompletion& completion, 
					  const std::string& filter_host, int filter_port, 
					  RedoPolicy* rp){
	  LOG(INFO)<<"ClientImpl::Incr(), start ns="<<ns<<" key="<<key<<" filter_host="<<filter_host
			<<" operand="<<s32operand_<<" default="<<s32initial_<<" ttl="<<u64ttl_;
	  boost::shared_ptr<Channel> channel;
	  std::string using_host;
	  int			using_port;
	  bool ret = GetChannel(filter_host, filter_port, using_host, using_port, channel);
	  if (ret){
		  /*AtomicIncrRequest* crq = new AtomicIncrRequest(this, ns, key, 
												  s32operand_, s32initial_, 
												  u64ttl_, completion, 
												  using_host, using_port, rp);*/

          boost::shared_ptr<AtomicIncrRequest> crq(new AtomicIncrRequest(this, ns, key, 
                                      s32operand_, s32initial_, 
                                      u64ttl_, completion, 
                                      using_host, using_port, rp));

		  channel->Post(crq,ldd::util::TimeDiff::Milliseconds(500), true);
		  return ldd::protocol::kOk;
	  }else{
		  delete rp;
          LOG(ERROR)<<"ClientImpl::Incr() GetChannel error";
		  return ldd::protocol::kNoConnection; 
	  }
}

// append
int ClientImpl::Append(const std::string& ns, const std::string& key, 
						uint8_t u8position, const std::string& content, 
						uint64_t u64ttl, const CasCompletion& completion){

	RedoPolicy* rp = redo_policy_->Copy();
	rp->FireTime(util::Time::CurrentMilliseconds());
	return Append(ns, key, u8position, content, u64ttl, completion, "",  0,  rp);
}

int ClientImpl::Append(const std::string& ns, const std::string& key, 
						uint8_t u8position, const std::string& content, 
						uint64_t u64ttl, const CasCompletion& completion,
						const std::string& filter_host, int filter_port,
						RedoPolicy* rp){
	LOG(INFO)<<"ClientImpl::Append(), start ns="<<ns<<" key="<<key<<" filter_host="<<filter_host
			<<" position="<<u8position<<" content="<<content<<" ttl="<<u64ttl;

	boost::shared_ptr<Channel> channel;
	std::string using_host;
	int			using_port;
	bool ret = GetChannel(filter_host, filter_port, using_host, using_port, channel);
	if (ret){
        boost::shared_ptr<AtomicAppendRequest> aar (
            new AtomicAppendRequest(this, ns, key, 
                    u8position, content, u64ttl, 
                    completion, using_host, using_port, rp));



		channel->Post(aar,ldd::util::TimeDiff::Milliseconds(500), true);
		return ldd::protocol::kOk;
	}else{
		delete rp;
        LOG(ERROR)<<"ClientImpl::Append() GetChannel error";
		return ldd::protocol::kNoConnection; 
	}
}

} // namespace ldd {
} // namespace client {
} // namespace raw {
