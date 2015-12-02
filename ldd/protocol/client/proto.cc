

#include "proto.h"
#include <glog/logging.h>
#include "ldd/protocol/coded_stream.h"

namespace ldd{
namespace protocol{


#define EQUAL_RET_VALUE(f, r) do { if( ( f ) ==false ) { LOG(ERROR)<<__FUNCTION__<<" : "<<__LINE__<<" stream opt error"; return (r); } } while(false)

GetMessage::GetMessage(){	
}
size_t GetMessage::ByteSize() const{
	int len =  sizeof(uint8_t) + name_space_.size();
	len += sizeof(uint16_t);
	len += key_.size();

	return len;
}
void GetMessage::SerializeToArray(void* data){
	char* cur = static_cast<char*>(data);
	StreamWriter::WriteU8(&cur, NULL, (uint8_t)name_space_.size());
	StreamWriter::WriteBytes(&cur, NULL, name_space_.data(), name_space_.size());

	StreamWriter::WriteU16(&cur, NULL, (uint16_t)key_.size() );
	StreamWriter::WriteBytes(&cur, NULL, key_.data(), key_.size());

	return ;
}

bool GetMessage::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data); 
	int left = size;
	uint8_t u8ns_len = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8ns_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u8ns_len, &name_space_), false);

    util::Slice s; uint16_t key_len = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &key_len), false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur, &left, key_len, &key_), false);

	return true;
}

CheckMessage::CheckMessage(){

}
size_t CheckMessage::ByteSize() const{
	int len =  sizeof(uint8_t) + name_space_.size();
	len += sizeof(uint16_t);
	len += key_.size();
	return len;
}
void CheckMessage::SerializeToArray(void* data){
	char* cur = static_cast<char*>(data);
	StreamWriter::WriteU8(&cur, NULL, (uint8_t)name_space_.size());
	StreamWriter::WriteBytes(&cur, NULL, name_space_.data(), name_space_.size());

	StreamWriter::WriteU16(&cur, NULL, (uint16_t)key_.size());
	StreamWriter::WriteBytes(&cur, NULL, key_.data(), key_.size());

	return ;
}

bool CheckMessage::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data);  
	int left = size;

	uint8_t u8ns_len = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8ns_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u8ns_len, &name_space_), false);
    util::Slice s;		 uint16_t key_len = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &key_len), false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur, &left, key_len, &key_), false);
	return true;
}


MutateMessage::MutateMessage(){
}
size_t MutateMessage::ByteSize() const{
	int len =  sizeof(uint8_t) + name_space_.size();
	len += sizeof(uint16_t);
	len += mutation_.key.size();
	len += sizeof(uint8_t);	// opttype
	len += sizeof(uint8_t);	// has_expected_version
	if (mutation_.has_expected_version){
		len += sizeof(int64_t);
	}
	len += sizeof(uint8_t);	// has_expected_val
	if (mutation_.has_expected_val){
		len += sizeof(uint32_t);
		len += mutation_.expected_value.size();
	}

	if (mutation_.opt_type == 1){
		len += sizeof(uint32_t);
		len += mutation_.value.size();
		len += sizeof(uint8_t);
		if (mutation_.has_ttl == 1){
			len += sizeof(uint64_t);
		}
	}

	return len;
}

void MutateMessage::SerializeToArray(void* data){
	char* cur = static_cast<char*>(data);

	StreamWriter::WriteU8(&cur, NULL, (uint8_t)name_space_.size());
	StreamWriter::WriteBytes(&cur, NULL, name_space_.data(), name_space_.size());

	StreamWriter::WriteU16(&cur, NULL, (uint16_t)mutation_.key.size());
	StreamWriter::WriteBytes(&cur, NULL, mutation_.key.data(), mutation_.key.size());
	StreamWriter::WriteU8(&cur, NULL, mutation_.opt_type);
	StreamWriter::WriteU8(&cur, NULL, mutation_.has_expected_version);
	if (mutation_.has_expected_version == 1){
		StreamWriter::WriteU64(&cur, NULL, mutation_.expected_version);
	}
	StreamWriter::WriteU8(&cur, NULL, mutation_.has_expected_val);
	if (mutation_.has_expected_val == 1){
		StreamWriter::WriteU32(&cur, NULL, (uint32_t)mutation_.expected_value.size());
		StreamWriter::WriteBytes(&cur, NULL, mutation_.expected_value.data(), mutation_.expected_value.size());
	}

	if (mutation_.opt_type == 1){
		StreamWriter::WriteU32(&cur, NULL, (uint32_t)mutation_.value.size());
		StreamWriter::WriteBytes(&cur, NULL, mutation_.value.data(), mutation_.value.size());

		StreamWriter::WriteU8(&cur, NULL, mutation_.has_ttl);
		if (mutation_.has_ttl == 1){
			StreamWriter::WriteU64(&cur, NULL, mutation_.ttl);
		}
	}// if (key_value_col_.OptType == 1){
	return ;
}

bool MutateMessage::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data);  
	int left = size;
	uint8_t u8ns_len = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8ns_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u8ns_len, &name_space_), false);

	uint16_t key_len = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &key_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, key_len, &mutation_.key), false);
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &mutation_.opt_type),	false);
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &mutation_.has_expected_version),	false);
	if(mutation_.has_expected_version == 1){
		uint64_t version = 0;
		EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &version),	false);
		mutation_.expected_version = version;
	}
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &mutation_.has_expected_val),	false);
	if (mutation_.has_expected_val == 1){
		uint32_t value_len = 0;
		EQUAL_RET_VALUE(StreamReader::ReadU32(&cur, &left, &value_len),	false);
		EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, value_len, &mutation_.expected_value), false);
	}

	if (mutation_.opt_type == 1){
		uint32_t value_len = 0;
		EQUAL_RET_VALUE(StreamReader::ReadU32(&cur, &left, &value_len),	false);
		EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, value_len, &mutation_.value), false);
		EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &mutation_.has_ttl),	false);
		if (mutation_.has_ttl == 1){
			EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &mutation_.ttl),	false);
		}
	}
	return true;
}

ListMessage::ListMessage() :  u32limit_(0u), u8position_(0u){
}
size_t ListMessage::ByteSize( ) const{
	int len =  sizeof(uint8_t) + name_space_.size()+sizeof(uint16_t) + key_.size()
		+ sizeof(u32limit_) + sizeof(u8position_);
	return len;
}
void ListMessage::SerializeToArray(void* data){
	char* cur = static_cast<char*>(data);
	StreamWriter::WriteU8(&cur, NULL, (uint8_t)name_space_.size());
	StreamWriter::WriteBytes(&cur, NULL, name_space_.data(), name_space_.size());

	StreamWriter::WriteU16(&cur, NULL, (uint16_t)key_.size());
	StreamWriter::WriteBytes(&cur, NULL, key_.data(), key_.size());

	StreamWriter::WriteU32(&cur, NULL, u32limit_);
	StreamWriter::WriteU8(&cur, NULL, u8position_);

	return ;
}


bool ListMessage::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data);  
	int left = size;
	uint8_t u8ns_len = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8ns_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u8ns_len, &name_space_), false);

	uint16_t key_len;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &key_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, key_len, &key_), false);

	EQUAL_RET_VALUE(StreamReader::ReadU32(&cur, &left, &u32limit_),	false);
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8position_),	false);

	return true;
}


AtomicIncrMessage::AtomicIncrMessage()
	:  s32operand_(0u), u64ttl_(0u){
}
size_t AtomicIncrMessage::ByteSize() const{
	int len = 0;
	len =  sizeof(uint8_t) + name_space_.size() + sizeof(uint16_t) + key_.size() +
		sizeof(s32operand_) + sizeof(s32initial_) + sizeof(u64ttl_);
	return len;
}
void AtomicIncrMessage::SerializeToArray(void* data){
	char* cur = static_cast<char*>(data);

	StreamWriter::WriteU8(&cur, NULL, (uint8_t)name_space_.size());
	StreamWriter::WriteBytes(&cur, NULL, const_cast<char*>(name_space_.data()), name_space_.size());

	StreamWriter::WriteU16(&cur, NULL, (uint16_t)key_.size());
	StreamWriter::WriteBytes(&cur, NULL, const_cast<char*>(key_.data()), key_.size());

	StreamWriter::WriteU32(&cur, NULL, s32operand_);
    StreamWriter::WriteU32(&cur, NULL, s32initial_);
	StreamWriter::WriteU64(&cur, NULL, u64ttl_);
	return ;
}

bool AtomicIncrMessage::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data); 
	int left = size;

	uint8_t u8ns_len = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8ns_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u8ns_len, &name_space_), false);

	uint16_t u16key_len = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16key_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u16key_len, &key_), false);

	uint32_t v = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU32(&cur, &left, &v),	false);
	s32operand_ = v;
    EQUAL_RET_VALUE(StreamReader::ReadU32(&cur, &left, &v), false);
	s32initial_ = v;
	EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &u64ttl_),	false);

	return true;
}



AtomicAppendMessage::AtomicAppendMessage()
	:  u8position_(0u),   u64ttl_(0u){

}

size_t AtomicAppendMessage::ByteSize() const{
	int len =  sizeof(uint8_t) + name_space_.size()+ sizeof(uint16_t) + key_.size() +
		sizeof(u8position_) + sizeof(uint32_t) +content_.size()  + sizeof(u64ttl_);
	return len;
}
void AtomicAppendMessage::SerializeToArray(void* data){
	char* cur = static_cast<char*>(data);
	StreamWriter::WriteU8(&cur, NULL, (uint8_t)name_space_.size());
	StreamWriter::WriteBytes(&cur, NULL, name_space_.data(), name_space_.size());

	StreamWriter::WriteU16(&cur, NULL, (uint16_t)key_.size());
	StreamWriter::WriteBytes(&cur, NULL, key_.data(), key_.size());

	StreamWriter::WriteU8(&cur, NULL, u8position_);

	StreamWriter::WriteU32(&cur, NULL, (uint32_t)content_.size());
	StreamWriter::WriteBytes(&cur, NULL, content_.data(), content_.size());

	StreamWriter::WriteU64(&cur, NULL, u64ttl_);
	return ;
}

bool AtomicAppendMessage::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data); 
	int left = size;
	uint8_t u8ns_len = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8ns_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u8ns_len, &name_space_), false);

	uint16_t u16key_len = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16key_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u16key_len, &key_), false);

	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8position_),	false);

	uint32_t slice_len;
	EQUAL_RET_VALUE(StreamReader::ReadU32(&cur, &left, &slice_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, slice_len, &content_), false);

	EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &u64ttl_),	false);

	return true;
}



#undef EQUAL_RET_VALUE

}//namespace protocol
}//namespace ldd




