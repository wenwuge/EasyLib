#include <boost/bind.hpp>
#include <glog/logging.h>

#include "client.h"
#include "internal/client_impl.h"

namespace ldd {
namespace client {
namespace raw {

/************************************************************************/
/* for put                                                              */
/************************************************************************/
Mutation::Mutation(const std::string& ns,
				const std::string& key,
				const std::string& value,
				uint64_t		   ttl):
	name_space_(ns),	key_(key),    opt_type_(1),
	has_expected_version_(0), expected_version_(0), 
	has_expected_val_(0),	  value_(value), 
	has_ttl_(0), ttl_(ttl){
	if (ttl > 0){
		has_ttl_ = 1;
	}
}

Mutation::Mutation(const std::string& ns,
				const std::string& key,
				const std::string& value,
				uint64_t		   ttl,
				int64_t			   expected_version) : 
	name_space_(ns),	key_(key),    opt_type_(1),
	has_expected_version_(1), expected_version_(expected_version), 
	has_expected_val_(0),	  value_(value), 
	has_ttl_(0), ttl_(ttl){
	if (ttl > 0){
		has_ttl_ = 1;
	}
}

Mutation::Mutation(const std::string& ns,
				const std::string& key,
				const std::string& value,
				uint64_t		   ttl,
				const std::string& expected_value): 
	name_space_(ns),	key_(key),    opt_type_(1),
	has_expected_version_(0), expected_version_(0), 
	has_expected_val_(1), expected_value_(expected_value),
	value_(value), 
	has_ttl_(0), ttl_(ttl){
	if (ttl > 0){
		has_ttl_ = 1;
	}
}

Mutation::Mutation(const std::string& ns,
	const std::string&  key,
	const std::string&  value,
	uint64_t			ttl,
	int64_t			    expected_version,
	const std::string&  expected_value) : 
	name_space_(ns),	key_(key),    opt_type_(1),
	has_expected_version_(1), expected_version_(expected_version), 
	has_expected_val_(1), expected_value_(expected_value),
	value_(value), 
	has_ttl_(0), ttl_(ttl){
	if (ttl > 0){
		has_ttl_ = 1;
	}
}

/************************************************************************/
/* for delete                                                           */
/************************************************************************/
Mutation::Mutation(const std::string& ns, 
				const std::string& key): 
	name_space_(ns),	key_(key),    opt_type_(2),
	has_expected_version_(0), expected_version_(0), 
	has_expected_val_(0), 
	has_ttl_(0), ttl_(0){
}


Mutation::Mutation(const std::string& ns, 
	const std::string&  key,
	int64_t expected_version):
	name_space_(ns),	key_(key),    opt_type_(2),
	has_expected_version_(1), expected_version_(expected_version), 
	has_expected_val_(0), 
	has_ttl_(0), ttl_(0){
}

Mutation::Mutation(const std::string& ns, 
	const std::string&  key,
	const std::string& expected_value):
	name_space_(ns),	key_(key),    opt_type_(2),
	has_expected_version_(0), expected_version_(0), 
	has_expected_val_(1), expected_value_(expected_value),
	has_ttl_(0), ttl_(0){
}

Mutation::Mutation(const std::string&  ns, 
	const std::string&	key,
	int64_t				expected_version,
	const std::string&  expected_value) : 
	name_space_(ns),	key_(key),    opt_type_(2),
	has_expected_version_(1), expected_version_(expected_version), 
	has_expected_val_(1), expected_value_(expected_value),
	has_ttl_(0), ttl_(0){
}



Client::Client(){

}

Client::~Client(){

}

Client* NewClient(net::Client* client, 
                  net::EventLoop* event_loop,
				  Provider* provider, 
				  const Options& option){

	if (!client|| !provider){
		LOG(ERROR)<<"Client NewClient() provider client NULL error";
		return NULL;
	}
	
    ClientImpl*  impl = new ClientImpl(client, event_loop, provider, option);
	if (!impl){
		LOG(ERROR)<<"Client* NewClient() error create client_impl";
		return impl;
	}

	/*provider->RegisterNotify(boost::bind(&ClientImpl::NodeListChangedNotify, 
                                            impl, _1, _2, _3));*/
	/*bool ret = provider->Init();
	if (!ret){
		LOG(ERROR)<<"Client* NewClient() provider initialize error";
        delete impl;
		return NULL;
	}*/
    if (!impl->Init()) {
        delete impl;
        return NULL;
    }

    return impl;
}

} // namespace ldd {
} // namespace client {
} // namespace raw {

