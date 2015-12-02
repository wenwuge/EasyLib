#ifndef __LDD_SYN_CLIENT__H_
#define __LDD_SYN_CLIENT__H_


#include <stdint.h>
#include <string>
#include <map>


namespace ldd{
    namespace client{
        namespace raw{
            class Provider;
        }
    }
}


namespace ldd {
namespace client {
namespace syn {

enum ErrorCode{
	kOk						= 0,

	kNotEnoughMem				= -808,
	kUnknown					= -809,
	kDbLoadError				= -810,
	kCacheLoadError				= -811,
	kInvalidParameter			= -812
};


struct Option{
	Option() : pulse_interval(100),
        connect_timeout(10), 
        resolve_timeout(10),
        recv_timeout(100),
        done_timeout(300),
        ttl(0),
		retry_interval(0), 
        zk_timeout(200){
    }

    int     pulse_interval  ;   // default = 100ms;        
    int     connect_timeout ;   // default = 10ms;
    int     resolve_timeout ;   // default = 10ms;
    int     recv_timeout    ;   // default = 200ms         // for net
    int     done_timeout    ;   // default = 300ms         
    int     ttl;                // for example ttl=600000,  key will expired after 10minutes


    std::string name_space;
	uint32_t	retry_interval; // retry_interval: after first failed(milliseconds)
	std::string zk_host_port;	// for example : "127.0.0.1:80,127.0.0.2:81,127.0.0.3:82"
    std::string zk_base_path;   // default ""
    int         zk_timeout;     // default 200ms
};


class SynClient{
public:
	virtual ~SynClient();

	/*	
		if ok code=0,  if failed, code < 0 
	*/
    virtual int  Put(const std::string& key,const std::string& value) = 0;

	/*
		@param version : check the existed version of @key, if current version=@version, 
						  put this value, else return error code
	*/
	virtual int  Put(const std::string& key, const std::string& value, 
													int64_t version) = 0;

	/*
		@param version : check the exist value of @key, if current version=@version, 
			put this value, else return error code
	*/
	virtual int  Put(const std::string& key, const std::string& value, 
									const std::string& expect_value) = 0;

	/*
		pair->first: value, pair->second:error code:
		<key, <value, error_code> >
	*/
	virtual int  Put(std::map<std::string, 
							std::pair<std::string, uint64_t> >& kvs) = 0;

	virtual int  Get(const std::string&key, std::string* value)= 0;

	/*
		pair->first: value, pair->second:error code
		<key, <value, error_code> >
	*/
	virtual int  Get(std::map<std::string, 
							std::pair<uint64_t, std::string> >& kvs)= 0;

	virtual int  Delete(const std::string& key)= 0;

	/*
		@param limit : return <=limit key-value
		@param position : =1 return current key[in], =2 not return key[in]
	*/
	virtual int List(const std::string& key, uint32_t limit, uint8_t position)=0;
	
};

/*
  create a synchronous client with option
			return a client pointer(sc), if return code=0; otherwise *sc==NULL;
  @param provider:get proxy list(host and port);   1:set a static list, 
			2:get list from zk
*/
int NewSynClient(SynClient** sc, raw::Provider* provider, const Option& option);

} // namespace ldd {
} // namespace client {
} // namespace syn {



#endif 
