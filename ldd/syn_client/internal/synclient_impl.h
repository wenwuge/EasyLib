#ifndef __LDD_CLIENT_SYN_CLIENT_IMPL__H_
#define __LDD_CLIENT_SYN_CLIENT_IMPL__H_


namespace ldd {
namespace client {
namespace raw {

			class Client;
			class RedoPolicySimple;

		}
	}
}

namespace ldd {
namespace net{
	class EventLoopThread;
	class Client;
}
}

using  ldd::client::raw::Client;
using  ldd::client::raw::RedoPolicySimple;
using  ldd::net::EventLoopThread;

#include "../syn_client.h"

namespace ldd {
namespace client {
namespace syn {



class SynClientImpl : public SynClient{
public:
	SynClientImpl(const std::string& ns);
	~SynClientImpl();
	int Init(const Option& option, raw::Provider* provider);
    virtual int  Put(const std::string& key,const std::string& value) ;
	virtual int  Put(const std::string& key, const std::string& value, 
											int64_t version) ;
	virtual int  Put(const std::string& key, const std::string& value, 
										const std::string& expect_value) ;
	virtual int  Put(std::map<std::string, 
									std::pair<std::string, uint64_t> >& kvs) ;


	virtual int  Get(const std::string&key, std::string* value) ;
	virtual int  Get(std::map<std::string, 
						std::pair<uint64_t, std::string> >& kvs);
	virtual int  Delete(const std::string& key) ;

	virtual int  List(const std::string& key, uint32_t limit, uint8_t position);

public:
	std::string& NameSpace();
private:
	int DoPut(ldd::client::raw::Mutation& mu);
private:
    ldd::net::EventLoopThread* event_thread_loop_;
    ldd::net::Client*		net_client_;
	Client*			ldd_client_;
	RedoPolicySimple* rp_;
	uint64_t		ttl_;
	std::string		name_space_;
    client::raw::Provider* provider_;
};

} // namespace ldd {
} // namespace client {
} // namespace syn {

#endif



