#include <boost/bind.hpp>
#include <glog/logging.h>
#include <boost/bind.hpp>


#include "ldd/util/time.h"
#include "ldd/net/event_loop.h"
#include "ldd/net/client.h"
#include "ldd/client/client.h"
#include "ldd/client/provider.h"
#include "ldd/client/redo_simple_policy.h"


#include "synclient_impl.h"
#include "syn_completion.h"


#if 0
using namespace ldd::client::raw;
using namespace ldd::net;
#endif

namespace ldd {
namespace client {
namespace syn {

SynClient::~SynClient(){
}

int NewSynClient(SynClient** impl, raw::Provider* provider, 
                 const Option& option){
	SynClientImpl* s = new SynClientImpl(option.name_space);

	int ret = s->Init(option, provider);
	if ( ret != 0){
		LOG(ERROR)<<" SynClientImpl init error";
		delete s;
		return ret;
	}

    LOG(INFO)<<"NewSynClient() name_space="<<option.name_space;
	
	*impl = s;
	return 0;
}

SynClientImpl::SynClientImpl(const std::string& ns) :event_thread_loop_(NULL),
		net_client_(NULL), ldd_client_(NULL), rp_(NULL), 
		ttl_(0), name_space_(ns),provider_(NULL){
}

SynClientImpl::~SynClientImpl(){
	event_thread_loop_->Stop();
	event_thread_loop_->Join();
    
	delete ldd_client_;
	delete rp_;
	delete event_thread_loop_;
    delete provider_;
}

int SynClientImpl::Init(const Option& opt, raw::Provider* provider) {      

    ldd::net::Client::Options net_options;
    net_options.pulse_interval  = opt.pulse_interval;        
    net_options.connect_timeout = opt.connect_timeout;
    net_options.resolve_timeout = opt.resolve_timeout;

    //net_options.notify_connected = NotifyConnected;
    //net_options.notify_connecting = NotifyConnecting;
    //net_options.notify_closed = NotifyClosed;

	ldd::client::raw::Options option ;
    option.recv_timeout = opt.recv_timeout;
    option.done_timeout = opt.done_timeout;

	rp_				= new RedoPolicySimple(0, opt.retry_interval);   // todo should parameterized
	option.rp		= rp_;
	option.time_out = opt.retry_interval;  // milliseconds		// retry when failed in the first
    if (opt.ttl > 0){
        ttl_ = opt.ttl;
        LOG(INFO)<<"SynClientImpl::Init() ttl="<<ttl_;
    }
	
	event_thread_loop_  = new EventLoopThread;
    net_client_		    = new ldd::net::Client(net_options);
	//event_thread_loop_->Start();

    if (provider == NULL){
        provider_ = new ldd::client::raw::ProxyZkProvider(opt.zk_host_port, 
            opt.zk_base_path, opt.zk_timeout);	
        provider_->Init();
        provider = provider_;
        LOG(INFO)<<"SynClientImpl::Init() create provider internally";
    }
	ldd_client_ = NewClient(net_client_, event_thread_loop_->event_loop(), 
                             provider, option);
	if (!ldd_client_){
		//event_thread_loop_->Stop();
		//event_thread_loop_->Join();
		//delete net_client_;
		LOG(ERROR)<<"client create error";
		return kCacheLoadError;
	}
    event_thread_loop_->Start();
	return 0;
}

int SynClientImpl::Put(const std::string& key,const std::string& value){
	int ret = 0;
    uint64_t ttl = 0;
    if (ttl_ > 0){
        ttl = (Time::CurrentMilliseconds() + ttl_) * 1000;
    }

    ldd::client::raw::Mutation mutation(NameSpace(), key, value, ttl);
    DLOG(INFO)<<"SynClientImpl::Put() key="<<key<<" value_size="<<value.size();
	SynCompletion syn_completion_;
	ret = ldd_client_->Mutate(mutation, boost::bind(
                &SynCompletion::MutateCompletion, &syn_completion_, _1));
	if (ret == 0 ){
		syn_completion_.WaitCompletion();
	}else{
		syn_completion_.MutateCompletion(ret);
	}

	int64_t version = syn_completion_.version();
	if (version >= 0){
		ret = 0;
        DLOG(INFO)<<"SynClientImpl::Put() ok!!!  ret="<<ret<<" key="<<key<<" value_size="
            <<value.size();
	}else{
		ret = version;
        LOG(ERROR)<<"SynClientImpl::Put() error!!! ret="<<ret<<" key="<<key<<" value_size="
            <<value.size();
	}
	
	return ret;
}

int  SynClientImpl::DoPut(ldd::client::raw::Mutation& mu){
	int ret = 0;
	SynCompletion syn_completion_;
	ret = ldd_client_->Mutate(mu, boost::bind(
					&SynCompletion::MutateCompletion, &syn_completion_, _1));
	if (ret == 0 ){
		syn_completion_.WaitCompletion();
	}else{
		syn_completion_.MutateCompletion(ret);
	}
	int64_t version = syn_completion_.version();

	return (int)version;
}

int  SynClientImpl::Put(const std::string& key, const std::string& value, 
				 int64_t version) {
	int ret = 0;
	uint64_t ttl = 0;
	if (ttl_ > 0){
		ttl = (Time::CurrentMilliseconds() + ttl_) * 1000;
	}

	ldd::client::raw::Mutation mutation(NameSpace(), key, value, ttl,version);
	DLOG(INFO)<<"SynClientImpl::Put() key="<<key<<" value_size="<<value.size();

	ret = DoPut(mutation);
	if (ret >= 0){
		ret = 0;
		DLOG(INFO)<<"SynClientImpl::Put() ok!!!  ret="<<ret<<" key="<<key<<" value_size="
			<<value;
	}else{
		LOG(ERROR)<<"SynClientImpl::Put() error!!! ret="<<ret<<" key="<<key<<" value_size="
			<<value;
	}
	return ret;
}

int  SynClientImpl::Put(const std::string& key, const std::string& value, 
				 const std::string& expect_value) {
	 uint64_t ttl = 0;
	 if (ttl_ > 0){
		 ttl = (Time::CurrentMilliseconds() + ttl_) * 1000;
	 }

	 ldd::client::raw::Mutation mutation(NameSpace(), key, value, ttl, expect_value);
	 DLOG(INFO)<<"SynClientImpl::Put() key="<<key<<" value_size="<<value.size();

	 return DoPut(mutation);
}

int  SynClientImpl::Put(std::map<std::string, std::pair<std::string, uint64_t> >& kvs) {
	int ret = 0;
	uint64_t ttl = 0;
	if (ttl_ > 0){
		ttl = (Time::CurrentMilliseconds() + ttl_) * 1000;
	}
	
	SynCompletion * sc = new SynCompletion[kvs.size()];
	CountDownLatch latch(kvs.size());
	
	int index=0;
	std::map<std::string, std::pair<std::string, uint64_t> >::iterator itr ;
	for(itr=kvs.begin(); itr != kvs.end(); itr++){
		ldd::client::raw::Mutation mutation(NameSpace(), 
							itr->first, itr->second.first, ttl);
		sc[index].SetInfo(itr->first, &latch);
		ret = ldd_client_->Mutate(mutation, boost::bind(
							&SynCompletion::MutateCompletion, &sc[index], _1));
		if (ret != 0){
			sc[index].MutateCompletion(ret);
		}
		
		index++;
	}
	latch.Wait();

	for (size_t j=0; j<kvs.size(); j++ ){
		 itr = kvs.find(sc[j].key());
		 if (itr !=  kvs.end()){
			 std::pair<std::string, uint64_t>& pair = itr->second;
			 pair.second = sc[j].version();
		 }	 
	}
	return 0;
}


int  SynClientImpl::List(const std::string& key, uint32_t limit, uint8_t position){
	
	return 0;
}


int SynClientImpl::Get(const std::string&key, std::string* value){
	int ret = 0;
    
	SynCompletion syn_completion_;
    DLOG(INFO)<<"SynClientImpl::Get() key="<<key;
	ret = ldd_client_->Get(NameSpace(), key, boost::bind(&SynCompletion::DataCompletion, 
        &syn_completion_, _1, _2, _3));
	if (ret == 0 ){
		syn_completion_.WaitCompletion();
	}else{
		syn_completion_.DataCompletion(ret, "", 0);
	}

	*value  = syn_completion_.value();

	int64_t version = syn_completion_.version();
	if (version >= 0){
		ret = 0;
        DLOG(INFO)<<"SynClientImpl::Get() ok!!! ret="<<ret<<" key="<<key<<" value_size="
            <<(*value).size();
	}else{
		ret = version;
        LOG(WARNING)<<"SynClientImpl::Get() ret="<<ret<<" key="<<key<<" value_size="
            <<(*value).size();
	}
	return ret;
}

int  SynClientImpl::Get(std::map<std::string, 
				 std::pair<uint64_t, std::string> >& kvs){
	 int ret = 0;
	 uint64_t ttl = 0;
	 if (ttl_ > 0){
		 ttl = (Time::CurrentMilliseconds() + ttl_) * 1000;
	 }

	 SynCompletion * sc = new SynCompletion[kvs.size()];
	 CountDownLatch latch(kvs.size());

	 int index=0;
	 std::map<std::string, std::pair<uint64_t, std::string> >::iterator itr ;
	 for(itr=kvs.begin(); itr != kvs.end(); itr++){
		 sc[index].SetInfo(itr->first, &latch);
		 ret = ldd_client_->Get(NameSpace(), itr->first, boost::bind(&SynCompletion::DataCompletion, 
			 &sc[index], _1, _2, _3));

		 if (ret != 0){
			 sc[index].DataCompletion(ret, "", 0);
		 }

		 index++;
	 }
	 latch.Wait();

	 for (size_t j=0; j<kvs.size(); j++ ){
		 itr = kvs.find(sc[j].key());
		 if (itr !=  kvs.end()){
			 std::pair<uint64_t, std::string>& pair = itr->second;
			 pair.first = sc[j].version();
			 pair.second = sc[j].value();
		 }	 
	 }
	 return 0;
}

int SynClientImpl::Delete(const std::string& key){
	int ret = 0;
    LOG(INFO)<<"SynClientImpl::Delete() key="<<key;
	ldd::client::raw::Mutation mutaion(NameSpace(), key);
	SynCompletion syn_completion_;
	ret = ldd_client_->Mutate(mutaion, boost::bind(&SynCompletion::MutateCompletion, 
        &syn_completion_, _1));
	if (ret == 0 ){
		syn_completion_.WaitCompletion();
	}else{
		syn_completion_.MutateCompletion(ret);
	}

	int64_t version = syn_completion_.version();
	if (version >= 0){
		ret = 0;
        LOG(INFO)<<"SynClientImpl::Delete() ok!!! ret="<<ret<<" key="<<key;
	}else{
		ret = version;
        LOG(ERROR)<<"SynClientImpl::Delete() ret="<<ret<<" key="<<key;
	}
	
	return ret;
}

std::string& SynClientImpl::NameSpace(){
	return name_space_;
}

} // namespace ldd {
} // namespace client {
} // namespace syn {

