#include "ldd/protocol/coded_stream.h"
#include "ldd/protocol/server/store_proto.h"
#include <glog/logging.h>


namespace ldd{
namespace protocol{

#define EQUAL_RET_VALUE(f, r) do { if( ( f ) ==false ) { LOG(ERROR)<<__FUNCTION__<<" : "<<__LINE__<<" stream opt error"; return (r); } } while(false)
/************************************************************************/
/* Serialize or Parse message from  proxy to store	                    */
/************************************************************************/
GetMessageS::GetMessageS():  u16Ns_(0u){	
}
size_t GetMessageS::ByteSize() const{
	int len =  sizeof(uint16_t);
	len += + sizeof(uint16_t); // bucketid
	len += sizeof(uint16_t);
	len += key_.size();
	return len;
}
void GetMessageS::SerializeToArray(void* data){
	char* cur = static_cast<char*>(data);
	StreamWriter::WriteU16(&cur, NULL, u16Ns_);
	StreamWriter::WriteU16(&cur, NULL, u16BucketID_);
	StreamWriter::WriteU16(&cur, NULL, (uint16_t)key_.size());
	StreamWriter::WriteBytes(&cur, NULL, key_.data(), key_.size());

	return ;
}
bool GetMessageS::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data); 
	int left = size;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16Ns_),	false);

	uint16_t key_len=0;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16BucketID_),	false);
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &key_len), false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, key_len, &key_), false);
	return true;
}

CheckMessageS::CheckMessageS():  u16Ns_(0u){	
}
size_t CheckMessageS::ByteSize() const{
	int len =  sizeof(uint16_t) + sizeof(uint8_t);
	len += + sizeof(uint16_t); // bucketid
	len += sizeof(uint16_t);
	len += key_.size();
	return len;
}
void CheckMessageS::SerializeToArray(void* data){
	char* cur = static_cast<char*>(data);
	StreamWriter::WriteU16(&cur, NULL, u16Ns_);

	StreamWriter::WriteU16(&cur, NULL, u16BucketID_);
	StreamWriter::WriteU16(&cur, NULL, (uint16_t)key_.size());
	StreamWriter::WriteBytes(&cur, NULL, key_.data(), key_.size());

	return ;
}
bool CheckMessageS::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data); 
	int left = size;

	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16Ns_),	false);

	uint16_t key_len=0;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16BucketID_),	false);
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &key_len), false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, key_len, &key_), false);
	return true;
}


MutateMessageS::MutateMessageS()
	:u16Ns_(0u), u16BucketID_(0), 
	s64syn_version_(0), u8from_(0){
}
size_t MutateMessageS::ByteSize() const{
	int len =  sizeof(uint16_t) ;
	len += sizeof(uint16_t);		// bucketid
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
	len += sizeof(s64syn_version_);
	len += sizeof(u8from_);
	return len;
}

void MutateMessageS::SerializeToArray(void* data){
	char* cur = static_cast<char*>(data);

	StreamWriter::WriteU16(&cur, NULL, u16Ns_);
	StreamWriter::WriteU16(&cur, NULL, u16BucketID_);
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
	}

	StreamWriter::WriteU64(&cur, NULL, s64syn_version_);
	StreamWriter::WriteU8(&cur, NULL, u8from_);

	return ;
}
bool MutateMessageS::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data);  
	int left = size;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16Ns_),	false);

	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16BucketID_),	false);

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
	uint64_t v = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &v),	false);
	s64syn_version_ = v;

	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8from_),	false);
	return true;
}

ListMessageS::ListMessageS() : u16Ns_(0u), u32limit_(0u), u8position_(0u){
}
size_t ListMessageS::ByteSize() const{
	int len =  sizeof(uint16_t)  + sizeof(uint16_t) + key_.size()
		+ sizeof(u32limit_) + sizeof(u8position_);
	return len;
}
void ListMessageS::SerializeToArray(void* data){
	char* cur = static_cast<char*>(data);

	StreamWriter::WriteU16(&cur, NULL, u16Ns_);
	StreamWriter::WriteU16(&cur, NULL, (uint16_t)key_.size());
	StreamWriter::WriteBytes(&cur, NULL, key_.data(), key_.size());

	StreamWriter::WriteU32(&cur, NULL, u32limit_);
	StreamWriter::WriteU8(&cur, NULL, u8position_);
	return ;
}
bool ListMessageS::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data);  
	int left = size;

	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16Ns_),	false);
	uint16_t key_len;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &key_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, key_len, &key_), false);

	EQUAL_RET_VALUE(StreamReader::ReadU32(&cur, &left, &u32limit_),	false);
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8position_),	false);
	
	return true;
}

AtomicIncrMessageS::AtomicIncrMessageS()
	: u16Ns_(0u), u16BucketID_(0u), s32operand_(0u), 
		s32initial_(0),		u64ttl_(0u), 
		s64syn_version_(0), u8from_(0){
}
size_t AtomicIncrMessageS::ByteSize() const{
	int len = 0;
	len =  sizeof(uint16_t)  + sizeof(uint16_t) + sizeof(uint16_t) + key_.size() +
		sizeof(s32operand_) + sizeof(s32initial_)  + sizeof(u64ttl_);
	len += sizeof(s64syn_version_);
	len += sizeof(u8from_);
	return len;
}
void AtomicIncrMessageS::SerializeToArray(void* data){
	char* cur = static_cast<char*>(data);

	StreamWriter::WriteU16(&cur, NULL, u16Ns_);
	StreamWriter::WriteU16(&cur, NULL, u16BucketID_);

	StreamWriter::WriteU16(&cur, NULL, (uint16_t)key_.size());
	StreamWriter::WriteBytes(&cur, NULL, const_cast<char*>(key_.data()), key_.size());

	StreamWriter::WriteU32(&cur, NULL, s32operand_);
	StreamWriter::WriteU32(&cur, NULL, s32initial_);
	StreamWriter::WriteU64(&cur, NULL, u64ttl_);
	StreamWriter::WriteU64(&cur, NULL, s64syn_version_);
	StreamWriter::WriteU8(&cur, NULL, u8from_);
	return ;
}
bool AtomicIncrMessageS::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data); 
	int left = size;

	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16Ns_),	false);
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16BucketID_),	false);

	uint16_t u16key_len = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16key_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u16key_len, &key_), false);
	
	uint32_t v = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU32(&cur, &left, &v),	false);
	s32operand_ = v;
	EQUAL_RET_VALUE(StreamReader::ReadU32(&cur, &left, &v),	false);
	s32initial_ = v;
	EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &u64ttl_),	false);
	
	uint64_t v64 = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &v64),	false);
	s64syn_version_ = v64;

	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8from_),	false);
	return true;
}

AtomicAppendMessageS::AtomicAppendMessageS()
	:  u16Ns_(0u), u16BucketID_(0u), u8position_(0u),   u64ttl_(0u), s64syn_version_(0), u8from_(0){

}

size_t AtomicAppendMessageS::ByteSize() const{
	int len =  sizeof(uint16_t)  + sizeof(uint16_t) + sizeof(uint16_t) + key_.size() +
		sizeof(u8position_) + sizeof(uint32_t) +content_.size()  + sizeof(u64ttl_);
	len += sizeof(s64syn_version_);
	len += sizeof(u8from_);
	return len;
}
void AtomicAppendMessageS::SerializeToArray(void* data){
	char* cur = static_cast<char*>(data);

	StreamWriter::WriteU16(&cur, NULL, u16Ns_);
	StreamWriter::WriteU16(&cur, NULL, u16BucketID_);

	StreamWriter::WriteU16(&cur, NULL, (uint16_t)key_.size());
	StreamWriter::WriteBytes(&cur, NULL, key_.data(), key_.size());

	StreamWriter::WriteU8(&cur, NULL, u8position_);

	StreamWriter::WriteU32(&cur, NULL, (uint32_t)content_.size());
	StreamWriter::WriteBytes(&cur, NULL, content_.data(), content_.size());

	StreamWriter::WriteU64(&cur, NULL, u64ttl_);
	StreamWriter::WriteU64(&cur, NULL, s64syn_version_);
	StreamWriter::WriteU8(&cur, NULL, u8from_);
	return ;
}
bool AtomicAppendMessageS::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data); 
	int left = size;

	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16Ns_),	false);
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16BucketID_),	false);

	uint16_t u16key_len = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &u16key_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u16key_len, &key_), false);

	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8position_),	false);

	uint32_t slice_len;
	EQUAL_RET_VALUE(StreamReader::ReadU32(&cur, &left, &slice_len),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, slice_len, &content_), false);
	EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &u64ttl_),	false);

	uint64_t v = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &v),	false);
	s64syn_version_ = v;

	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8from_),	false);
	return true;
}

/************************************************************************/
/* message from store to store                                          */
/************************************************************************/
size_t  GetVersionMessageS::ByteSize() const{
	int len = 0;
	len += sizeof(uint8_t);
	len += host_port_.size();
	return len;
}
void GetVersionMessageS::SerializeToArray(void* data) {
	char* cur = static_cast<char*>(data);

	StreamWriter::WriteU8(&cur, NULL, (uint8_t)host_port_.size());
	StreamWriter::WriteBytes(&cur, NULL, host_port_.data(), host_port_.size());
	return ;
}
bool GetVersionMessageS::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data); 
	int left = size;
	uint8_t u8host = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8host),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u8host, &host_port_), false);
	return true;	
}

size_t  PrepareMessageS::ByteSize() const{
	int len = 0;
	len += sizeof(uint8_t);
	len += home_port_.size();
	len += sizeof(uint8_t);
	len += dest_host_port_.size();
	return len;
}
void PrepareMessageS::SerializeToArray(void* data) {
	char* cur = static_cast<char*>(data);

	StreamWriter::WriteU8(&cur, NULL, (uint8_t)home_port_.size());
	StreamWriter::WriteBytes(&cur, NULL, home_port_.data(), home_port_.size());

	StreamWriter::WriteU8(&cur, NULL, (uint8_t)dest_host_port_.size());
	StreamWriter::WriteBytes(&cur, NULL, dest_host_port_.data(), dest_host_port_.size());
	return ;
}
bool PrepareMessageS::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data); 
	int left = size;
	uint8_t u8host = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8host),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u8host, &home_port_), false);

	u8host = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8host),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u8host, &dest_host_port_), false);
	return true;
}

#undef EQUAL_RET_VALUE

}//namespace protocol
}//namespace ldd


