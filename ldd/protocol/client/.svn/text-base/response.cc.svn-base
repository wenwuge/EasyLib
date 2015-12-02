
#include <glog/logging.h>
#include "ldd/protocol/coded_stream.h"
#include "ldd/protocol/client/response.h"



namespace ldd{
namespace protocol{

#define EQUAL_RET_VALUE(f, r) do { if( ( f ) ==false ) { LOG(ERROR)<<__FUNCTION__<<__LINE__<<"stream opt error"; return (r); } } while(false)



/************************************************************************/
/* Serialize or Parse message from  CLient to proxy                     */
/************************************************************************/
GetOrListResponse::GetOrListResponse(){
}
size_t GetOrListResponse::ByteSize()const{
	int len = 0;

	len += sizeof(s64Version_);
	len += sizeof(uint16_t);
	len += key_.size();
	len += sizeof(uint32_t);
	len += value_.size();
	len += sizeof(u64ttl_);

	return len;
}
void GetOrListResponse::SerializeToArray(void* buffer){
	char* cur = static_cast<char*>(buffer);

	StreamWriter::WriteU64(&cur, NULL, s64Version_);
	StreamWriter::WriteU16(&cur, NULL, (uint16_t)key_.size());
	StreamWriter::WriteBytes(&cur, NULL, 
		const_cast<char*>(key_.data()), key_.size());

	StreamWriter::WriteU32(&cur, NULL, (uint32_t)value_.size());
	StreamWriter::WriteBytes(&cur, NULL, 
		const_cast<char*>(value_.data()), value_.size());	
	StreamWriter::WriteU64(&cur, NULL, u64ttl_);
	return ;
}

bool GetOrListResponse::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data); 
	int left = size;
	uint64_t version = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &version), false);
	s64Version_ = version;
	uint16_t key_len=0;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &key_len), false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur, &left, key_len, &key_), false);

	uint32_t value_len=0;
	EQUAL_RET_VALUE(StreamReader::ReadU32(&cur, &left, &value_len), false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur, &left, value_len, &value_), false);

	EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &u64ttl_), false);
	return true;
}


CheckResponse::CheckResponse(){
}
size_t CheckResponse::ByteSize()const{
	int len = 0;

	len += sizeof(s64Version_);
	len += sizeof(uint16_t);
	len += key_.size();
	len += sizeof(u64ttl_);

	return len;
}
void CheckResponse::SerializeToArray(void* buffer){
	char* cur = static_cast<char*>(buffer);
	StreamWriter::WriteU64(&cur, NULL, s64Version_);
	StreamWriter::WriteU16(&cur, NULL, (uint16_t)key_.size());
	StreamWriter::WriteBytes(&cur, NULL, 
		const_cast<char*>(key_.data()), key_.size());
	StreamWriter::WriteU64(&cur, NULL, u64ttl_);
	return ;
}
bool CheckResponse::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data); 
	int left = size;
	uint64_t version = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &version), false);
	s64Version_ = version;
	uint16_t key_len=0;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &key_len), false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur, &left, key_len, &key_), false);
	EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &u64ttl_), false);


	return true;
}


MutateResponse::MutateResponse(){
}
size_t MutateResponse::ByteSize()const{
	int len = 0;

	len += sizeof(s64Version_);

	return len;
}
void MutateResponse::SerializeToArray(void* buffer){
	char* cur = static_cast<char*>(buffer);

	StreamWriter::WriteU64(&cur, NULL, s64Version_);
	return ;
}
bool MutateResponse::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data); 
	int left = size;

	uint64_t version = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &version), false);
	s64Version_ = version;

	return true;
}


CasResponse::CasResponse(){
}
size_t CasResponse::ByteSize()const{
	int len = 0;
	len += sizeof(uint64_t);
	len += sizeof(uint16_t);
	len += key_.size();
	len += sizeof(uint32_t);
	len += value_.size();

	return len;
}
void CasResponse::SerializeToArray(void* buffer){
	char* cur = static_cast<char*>(buffer);

	StreamWriter::WriteU64(&cur, NULL, s64Version_);
	StreamWriter::WriteU16(&cur, NULL, (uint16_t)key_.size());
	StreamWriter::WriteBytes(&cur, NULL, 
		const_cast<char*>(key_.data()), key_.size());

	StreamWriter::WriteU32(&cur, NULL, (uint32_t)value_.size());
	StreamWriter::WriteBytes(&cur, NULL, 
		const_cast<char*>(value_.data()), value_.size());

	return ;
}
bool CasResponse::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data); 
	int left = size;


	uint64_t version = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &version), false);
	s64Version_ = version;

	uint16_t key_len=0;
	EQUAL_RET_VALUE(StreamReader::ReadU16(&cur, &left, &key_len), false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur, &left, key_len, &key_), false);

	uint32_t value_len=0;
	EQUAL_RET_VALUE(StreamReader::ReadU32(&cur, &left, &value_len), false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur, &left, value_len, &value_), false);
	return true;
}


GetVersionResponse::GetVersionResponse():s64Version_(0u){
}
size_t GetVersionResponse::ByteSize()const{
	int len = 0;
	len = sizeof(s64Version_);
	len += sizeof(uint8_t);
	len += home_host_port_.size();
	len += sizeof(uint8_t);
	len += dest_host_port_.size();
	return len;
}
void GetVersionResponse::SerializeToArray(void* buffer){
	char* cur = static_cast<char*>(buffer);

	StreamWriter::WriteU64(&cur, NULL, s64Version_);
	StreamWriter::WriteU8(&cur, NULL, (uint8_t)home_host_port_.size());
	StreamWriter::WriteBytes(&cur, NULL, home_host_port_.data(), home_host_port_.size());
	StreamWriter::WriteU8(&cur, NULL, 
		(uint8_t)dest_host_port_.size());
	StreamWriter::WriteBytes(&cur, NULL, 
		dest_host_port_.data(), dest_host_port_.size());
	return ;
}

bool GetVersionResponse::ParseFromArray(const void* data, int size){
	const char* cur = static_cast<const char*>(data); 
	int left = size;	
	uint64_t version = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU64(&cur, &left, &version), false);
	s64Version_ = version;

	uint8_t u8host = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8host),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u8host, &home_host_port_), false);

	u8host = 0;
	EQUAL_RET_VALUE(StreamReader::ReadU8(&cur, &left, &u8host),	false);
	EQUAL_RET_VALUE(StreamReader::ReadBytes(&cur,&left, u8host, &dest_host_port_), false);
	return true;
}


}//namespace protocol
}//namespace ldd


