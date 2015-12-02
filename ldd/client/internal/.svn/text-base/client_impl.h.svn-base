#ifndef __LDD_CLIENT_RAW_CLIENT_IMPLE__H_
#define __LDD_CLIENT_RAW_CLIENT_IMPLE__H_

#include "ldd/util/mutex.h"
#include "ldd/net/client.h"
#include "ldd/net/channel.h"
#include "ldd/net/outgoing_msg.h"

#include "../client.h"
#include "../provider.h"


namespace ldd {
namespace client {
namespace raw {

class RedoPolicy;
class ClientImpl : public Client{
public:
    ClientImpl(net::Client* client, 
               net::EventLoop* event_loop,
				Provider *provider, 
				const Options &option);
    ~ClientImpl();
    bool Init();

    int get_recv_timeout(){
        return option_.recv_timeout;
    }

    int get_done_timeout(){
        return option_.done_timeout;
    }

	virtual int Get(const std::string& ns, const std::string& key, 
					const DataCompletion& completion);

	virtual int Check(const std::string& ns,const std::string& key, 
					const CheckCompletion& completion) ;

	virtual int List(const std::string& ns, const std::string& key,
						uint32_t u32limit_, uint8_t	u8position_,  
						const DataCompletion& completion);

	virtual int Mutate(const Mutation& mutation, 
						const MutateCompletion& completion);

	virtual int Put(const std::string& ns, const std::string& key, 
					const std::string& value, 
					const MutateCompletion& completion) ;

	virtual int Delete(const std::string& ns, const std::string& key, 
						const MutateCompletion& completion);

	virtual int Incr(const std::string& ns,const std::string& key,
						int32_t s32operand_, int32_t	s32initial_,
						uint64_t u64ttl_, const CasCompletion& completion);

	virtual int Append(const std::string& ns, const std::string& key, 
						uint8_t u8position, const std::string& content, 
						uint64_t u64ttl, const CasCompletion& completion);


public:
	int Get(const std::string&ns, const std::string& key, 
			const DataCompletion& completion, 
			const std::string& filter_host, int filter_port, RedoPolicy* rp);
	int Check(const std::string& ns,const std::string& key, 
			const CheckCompletion& completion, 
			const std::string& filter_host, int filter_port, RedoPolicy* rp);
	
	int List(const std::string& ns, const std::string& key,
			uint32_t u32limit_, uint8_t	u8position_,  
			const DataCompletion& completion, 
			const std::string& filter_host, int filter_port, RedoPolicy* rp);

	int Mutate(const Mutation& mutation, const MutateCompletion& completion, 
			const std::string& filter_host, int filter_port, RedoPolicy* rp);

	int Incr(const std::string& ns,const std::string& key,
			int32_t s32operand_, int32_t	s32initial_,
			uint64_t u64ttl_, const CasCompletion& completion, 
			const std::string& filter_host, int filter_port, RedoPolicy* rp);

	int Append(const std::string& ns, const std::string& key, 
			uint8_t u8position, const std::string& content, 
			uint64_t u64ttl, const CasCompletion& completion, 
			const std::string& filter_host, int filter_port, RedoPolicy* rp);

	bool GetChannel(const std::string& filter_host, const int filter_port,
		std::string& using_host, int& using_port,
		boost::shared_ptr<net::Channel>& channel);

	void NodeListChangedNotify(int err_code, courier::NodeChangedType type, 
								const std::string& node);

private:
	typedef std::pair<std::string, int> HostPort;

    net::Client*	client_;
    net::EventLoop* event_loop_;
    Provider*		provider_;
	Options			option_;
    
    int				proxy_index_;

	util::RWMutex	rwmtx_;
	std::map<HostPort, boost::shared_ptr<net::Channel> > nodes_;
    std::vector<HostPort>			ordered_nodes_;
	RedoPolicy*						redo_policy_; 	
    int             cb_id_;
};

} // namespace ldd {
} // namespace client {
} // namespace raw {




#endif // __LDD_CLIENT_RAW_CLIENT_IMPLE__H_


