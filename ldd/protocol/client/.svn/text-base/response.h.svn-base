
#ifndef __LDD__RESPONSE_MESSAGE_H_
#define __LDD__RESPONSE_MESSAGE_H_

#include <stdint.h>
#include <vector>
#include "ldd/util/slice.h"
#include "ldd/protocol/message.h"


namespace ldd{
namespace protocol{


class Response : public	Message{
public:
	Response(){}
	virtual	~Response(){};
private:
	Response(const Response&);
	const Response& operator=(const Response&);
};

/*
1.  Getting        (#define LDD_CLIENT_GET 0x9910)                  读取接口
Get(ns, list{key}) => { list{version, key, value}}

Request Body  =  u8NsLen + char[u8NsLen] + u8ListLen + stKey[u8ListLen]
				stKey = u16keyLen + char[u16keyLen]
Response Body =  u8ListLen + stReturn[u8ListLen]
				stReturn = u64Version + u16keyLen + char[u16keyLen] + u32ValueLen + char[u32ValueLen];   
error_code = u64Version>>7   if(error_code==0) ok value, else not ok,   下面同理  
*/
class GetOrListResponse  :  public Response{
public:
	GetOrListResponse();
protected:
	virtual size_t  ByteSize() const;
	virtual void SerializeToArray(void* data) ;	
	virtual bool ParseFromArray(const void* data, int size);

public:
	int64_t		s64Version_;
    util::Slice		key_;		// u16keyLen
	util::Slice		value_;		// u32ValueLen
	uint64_t	u64ttl_;
};

/*
Request Body  =  u8NsLen + char[u8NsLen] + u8ListLen + stKey[u8ListLen]
			stKey = u16keyLen + char[u16keyLen]
Response Body =  u8ListLen + stReturn[u8ListLen]
			stReturn = u64Version + u16keyLen + char[u16keyLen];   

*/
class CheckResponse  :  public Response{
public:
	CheckResponse();
protected:
	virtual size_t  ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);


public:
	int64_t s64Version_;
	util::Slice	 key_;		// u16keyLen
	uint64_t u64ttl_;
};

/*
Request Body =  u8NsLen + char[u8NsLen] + u8ListLen + stInput[u8ListLen]
						stInput = u16keyLen + char[u16keyLen] + stMutation
Response Body =  u8ListLen + u64Version[u8ListLen]   
*/
class MutateResponse  :  public Response{
public:
	MutateResponse();
protected:
	virtual size_t  ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);

public:
	int64_t s64Version_;
};


// response for AtomicAppendMessage and AtomicIncrMessage
class CasResponse  :  public Response{
public:
	CasResponse();
protected:
	virtual size_t  ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);
public:

	int64_t s64Version_;
	util::Slice	 key_;			//  u16keyLen + char[u16keyLen]
	util::Slice	 value_;			//   u32ValueLen + char[u32ValueLen]
};


/************************************************************************/
/* response of store 2 store                                            */
/************************************************************************/
class GetVersionResponse  :  public Response{
public:
	GetVersionResponse();
protected:
	virtual size_t  ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);
public:
	int64_t s64Version_;
	util::Slice	home_host_port_;
	util::Slice	dest_host_port_;	
};



}//namespace protocol
}//namespace ldd


#endif //__LDD__RESPONSE_MESSAGE_H_
