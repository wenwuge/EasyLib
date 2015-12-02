

#ifndef __LDD__PROTOCOL_PROTO_H_
#define __LDD__PROTOCOL_PROTO_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include "ldd/util/slice.h"
#include "ldd/protocol/message.h"


namespace ldd{
namespace protocol{


/************************************************************************/
/* Serialize message from Client  to proxy                              */
/************************************************************************/

class GetMessage : public Message{
public:
	GetMessage();
protected:
	virtual size_t ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);

public:
    util::Slice				name_space_;						// (u8NsLen)
    util::Slice				key_;				    // (u16KeyLen)
};


class CheckMessage : public Message{
public:
	CheckMessage();
protected:
	virtual size_t ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);

public:
    util::Slice				name_space_;					// (u8NsLen)
	util::Slice				key_;			// (u16KeyLen)
};

struct stMutation{	
	stMutation(){
		opt_type = 1;
		has_expected_version = 0;
		has_expected_val	= 0;
		expected_version	= 0;
		has_ttl				= 0;
		ttl					= 0;
	}
    util::Slice			 key;   //u16keylen

	uint8_t				 opt_type;	//1=Put; 2=Delete
	uint8_t              has_expected_version;       // 0 not check ; 1 check version
	int64_t              expected_version;
	uint8_t              has_expected_val;			 // 0 not check ; 1 check value
    util::Slice			 expected_value;			// u32ValueLen

    util::Slice			 value;		// put opt_type=1 才会有		// u32ValueLen
	uint8_t              has_ttl;	// put opt_type=1 才会有
	uint64_t             ttl;		// put opt_type=1 才会有
};

class MutateMessage : public Message{
public:
	MutateMessage();
protected:
	virtual size_t ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);

public:
    util::Slice					 name_space_;			// (u8NsLen)
	stMutation				 mutation_;    
};

class ListMessage : public Message{
public:
	ListMessage();
protected:
	virtual size_t ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);

public:
    util::Slice		name_space_;						// (u8NsLen)
    util::Slice		key_;								// u16KeyLen
	uint32_t		u32limit_;
	uint8_t			u8position_;							// position==1 从当前key开始，包含当前key
													// position==2 从当前key开始，不包含当前key
};

class AtomicIncrMessage : public Message {
public:
	AtomicIncrMessage();
protected:
	virtual size_t ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);

public:
    util::Slice		name_space_;		// (u8NsLen) ns len contained in the name_space
	util::Slice		key_;				// (u16KeyLen)
	int32_t			s32operand_;
	int32_t			s32initial_;			// (u32InitialLen)
	uint64_t		u64ttl_;
};

class AtomicAppendMessage : public Message{
public:
	AtomicAppendMessage();
protected:
	virtual size_t ByteSize() const;
	virtual void SerializeToArray(void* data) ;
	virtual bool ParseFromArray(const void* data, int size);

public:
    util::Slice		name_space_;		// (u8NsLen)
	util::Slice		key_;				// (u16KeyLen)
	uint8_t			u8position_;		//  u8position=1 在原值的前段增加; u8position=2 在原值的末尾增加; u32ttl=0 时，没有意义
    util::Slice		content_;				//  (u32SliceLen)
	uint64_t		u64ttl_;
};


}//namespace protocol
}//namespace ldd


#endif //__LDD__PROTOCOL_PROTO_H_


