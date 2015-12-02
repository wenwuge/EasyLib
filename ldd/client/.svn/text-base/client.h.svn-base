
#ifndef __LDD_CLIENT_RAW_CLIENT__H_
#define __LDD_CLIENT_RAW_CLIENT__H_

#include <tr1/functional>
#include <string>

namespace ldd {
	namespace net{
		class Client;
        class EventLoop;
	}
}

//using ldd::net::UpStream;


namespace ldd {
namespace client {
namespace raw {

/*1,负载均衡,轮巡
 * 2，地址列表，从zookeeper取得，测试环境使用固定地址即可
 * 3，
 * */ 


typedef std::tr1::function<void(int64_t version, 
						   const std::string& value, uint32_t ttl)>DataCompletion;

typedef std::tr1::function<void(int64_t version, 
								uint32_t ttl)>CheckCompletion;

typedef std::tr1::function<void(int64_t version)>MutateCompletion;

typedef std::tr1::function<void(int64_t version, 
								const std::string& value)>CasCompletion;


enum client_code{
    kOK = 0 ,

    kCancel = -800,
    kFailed = -801
};

class RedoPolicy;
struct Options {
	Options() : time_out(0), rp(NULL){

	}

    int     recv_timeout    ;   // default = 200ms         // for net
    int     done_timeout    ;   // default = 300ms         

	int64_t  time_out;
	RedoPolicy* rp;
};

class Mutation
{
public:
	/************************************************************************/
	/* for put                                                              */
	/************************************************************************/
	Mutation(const std::string& ns,   const std::string& key,
			const std::string& value, uint64_t		   ttl);

	Mutation(const std::string& ns,   const std::string& key,
			const std::string& value, uint64_t		   ttl,
			int64_t			   expected_version);  //check version

	Mutation(const std::string& ns,	  const std::string& key,
			const std::string& value, uint64_t		   ttl,
			const std::string& expected_value);		// check value

	Mutation(const std::string& ns,   const std::string&  key,
			const std::string& value, uint64_t			ttl,
			int64_t			    expected_version,	// check version 
			const std::string&  expected_value);    // check value

	/************************************************************************/
	/* for delete                                                            */
	/************************************************************************/
	Mutation(const std::string& ns, const std::string& key);


	Mutation(const std::string& ns, const std::string&  key,
			int64_t expected_version);				// check version

	Mutation(const std::string& ns, const std::string&  key,
			const std::string& expected_value);		// check value

	Mutation(const std::string&  ns,const std::string&	key,
			int64_t				expected_version,	// check version
			const std::string&  expected_value);	// check value

private:
	friend class MutateRequest;
	Mutation(const Mutation&);
	Mutation& operator=(const Mutation&);
	
	
	std::string			 name_space_;
	std::string			 key_;	

	uint8_t				 opt_type_;					 //1=Put; 2=Delete
	uint8_t              has_expected_version_;     // 0 not check ; 1 check version
	int64_t              expected_version_;
	uint8_t              has_expected_val_;			// 0 not check ; 1 check value
	std::string			 expected_value_;			// u32ValueLen

	std::string			 value_;					// put opt_type=1 才会有		
	uint8_t              has_ttl_;					// put opt_type=1 才会有
	uint64_t             ttl_;						// put opt_type=1 才会有
};

class Provider;


class Client {
public:
    Client();
    virtual ~Client();


	virtual int Get(const std::string& ns, const std::string& key, 
						const DataCompletion& completion) = 0;

	virtual int Check(const std::string& ns, const std::string& key, 
						const CheckCompletion& completion) = 0;
	virtual int Mutate(const Mutation& mutation, const MutateCompletion& completion) = 0;

	virtual int Put(const std::string& ns, const std::string& key, 
						const std::string& value, 
						const MutateCompletion& completion) = 0;

	virtual int Delete(const std::string& ns, const std::string& key, 
						const MutateCompletion& completion) = 0;

	virtual int List(const std::string& ns, const std::string& key,
						uint32_t u32limit_, uint8_t	u8position_,  
						const DataCompletion& completion) = 0;

	virtual int Incr(const std::string& ns, const std::string& key,
					  int32_t s32operand, int32_t	s32initial,
					  uint64_t u64ttl, const CasCompletion& completion) = 0;
	
	virtual int Append(const std::string& ns, const std::string& key, 
						uint8_t u8position, const std::string& content, 
						uint64_t u64ttl, const CasCompletion& completion) = 0;
 
};

Client* NewClient(net::Client* client, net::EventLoop* event_loop,
                  Provider* provider, const Options& option);

} // namespace ldd {
} // namespace client {
} // namespace raw {

#endif // __LDD_CLIENT_RAW_CLIENT__H_
