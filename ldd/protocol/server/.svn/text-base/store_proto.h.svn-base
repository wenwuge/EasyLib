
#ifndef __LDD__PROTOCOL_MSG_PROTO_H_
#define __LDD__PROTOCOL_MSG_PROTO_H_

#include <stdint.h>
#include <string>
#include <vector>
#include "ldd/util/slice.h"
#include "ldd/protocol/message.h"
#include "ldd/protocol/client/proto.h"


namespace ldd{
namespace protocol{

	/************************************************************************/
/* message from proxy to store                                          */
/************************************************************************/


/*Get(u8Compressed,ns, list{key}) => {u8Compressed, list{version, key, value}}

Request Body  =  u16Ns + u32BucketID + u8ListLen + stKey[u8ListLen]
			stKey = u16keyLen + char[u16keyLen]
Response Body =  u8ListLen + stReturn[u8ListLen]
				stReturn = u64Version + u16keyLen + char[u16keyLen] + u32ValueLen + char[u32ValueLen];   
			error_code = u64Version>>7   if(error_code==0) ok value, else not ok,   下面同理   */
class GetMessageS : public Message{
public:
	GetMessageS();
protected:
	virtual size_t  ByteSize() const;
	virtual void SerializeToArray(void* data) ;	
	virtual bool ParseFromArray(const void* data, int size);


public:

	uint16_t						u16Ns_;
	uint16_t						u16BucketID_;
    util::Slice							key_;		// uint16_t len
};

/*Request Body  =  u16Ns + u32BucketID + u8ListLen + stKey[u8ListLen]
               stKey = u16keyLen + char[u16keyLen]
         Response Body =  u8ListLen + stReturn[u8ListLen]
         stReturn = u64Version + u16keyLen + char[u16keyLen];   
*/
class CheckMessageS : public Message{
public:
	CheckMessageS();
protected:
	virtual size_t  ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);


public:
	uint16_t	u16Ns_;
	uint16_t	u16BucketID_;
    util::Slice		key_;
};

/*Request Body =  u16Ns + u32BucketID + u8ListLen + stInput[u8ListLen]
                     stInput = u16keyLen + char[u16keyLen] + stMutation
Response Body =  u8ListLen + u64Version[u8ListLen]     
*/
class MutateMessageS : public Message{
public:
	MutateMessageS();
protected:
	virtual size_t  ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);

public:
	uint16_t				u16Ns_;
	uint16_t				u16BucketID_;
	stMutation				mutation_;
	int64_t					s64syn_version_;			// 这个字段只为store之间通讯使用, 仅store内部使用
	uint8_t					u8from_;					// 这个字段只为store之间通讯使用, 仅store内部使用
};

/*Request Body =  u16Ns + u32BucketID + u64Start + u64End + u64Limit
								u64End=0时，没有意义； u64Limit=0时，没有意义
Response Body =  u8ListLen + stReturn[u8ListLen] 
                        stReturn = u64Version + u16keyLen + char[u16keyLen] + u32ValueLen + char[u32ValueLen]           */
class ListMessageS : public Message{
public:
	ListMessageS();
protected:
	virtual size_t  ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);

public:
	uint16_t		u16Ns_;
	util::Slice		key_;								// u16KeyLen
	uint32_t		u32limit_;
	uint8_t			u8position_;	// todo 是否需要设置为有效的初始值		// position==1 从当前key开始，包含当前key
													// position==2 从当前key开始，不包含当前key
};

/*Reqeust Body =  u16Ns + u32BucketID + u16keyLen + char[u16keyLen] + 
u64Operand + u32initialLen + char[u32initialLen] +  u32ttl
u32ttl=0时，没有意思
Response Body =  u64Version + u16keyLen + char[u16keyLen] + u32ValueLen + char[u32ValueLen]
*/
class AtomicIncrMessageS: public Message{
public:
	AtomicIncrMessageS();
protected:
	virtual size_t  ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);

public:
	uint16_t		u16Ns_;
	uint16_t		u16BucketID_;			
	util::Slice		key_;					// (u16KeyLen)
	int32_t			s32operand_;
	int32_t			s32initial_;				// (u32InitialLen)
	uint64_t		u64ttl_;
	int64_t			s64syn_version_;			// 这个字段只为store之间通讯使用, 仅store内部使用
	uint8_t			u8from_;					// 这个字段只为store之间通讯使用, 仅store内部使用
};

/*
Reqeust Body =  u16Ns + u32BucketID + u16keyLen + char[u16keyLen] + 
u8position + u32Slice + char[u32Slice] + u32ttl
u8position=1 在原值的前段增加; u8position=2 在原值的末尾增加; u32ttl=0 时，没有意思
Response BOdy =  u8ListLen + stReturn[u8ListLen]
stReturn = u64Version + u16keyLen + char[u16keyLen] + u32ValueLen + char[u32ValueLen]
*/
class AtomicAppendMessageS: public Message{
public:
	AtomicAppendMessageS();
protected:
	virtual size_t  ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);

public:
	uint16_t		u16Ns_;
	uint16_t		u16BucketID_;	
	util::Slice		key_;					//  (u16KeyLen)
	uint8_t			u8position_;			//  u8position=1 在原值的前段增加; u8position=2 在原值的末尾增加; u32ttl=0 时，没有意义
	util::Slice		content_;					//  (u32SliceLen)
	uint64_t		u64ttl_;
	int64_t			s64syn_version_;			// 这个字段只为store之间通讯使用, 仅store内部使用
	uint8_t			u8from_;					// 这个字段只为store之间通讯使用, 仅store内部使用
};



/************************************************************************/
/* message from store to store                                          */
/************************************************************************/
class GetVersionMessageS: public Message{		// should create a IncomingMsg
public:
	GetVersionMessageS(){};
protected:
	virtual size_t  ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);

public:
	util::Slice host_port_;		// uint8_t len
};

class PrepareMessageS: public Message{		
public:
	PrepareMessageS(){};
protected:
	virtual size_t  ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);

public:
	util::Slice	home_port_;			// uint8_t len 
	util::Slice	dest_host_port_;	// uint8_t len 
};





}//namespace protocol
}//namespace ldd


#endif //__LDD__PROTOCOL_MSG_PROTO_H_
