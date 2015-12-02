
#include "coded_stream.h"
#include <netinet/in.h>
#include <endian.h>
#include "glog/logging.h"

namespace ldd{
namespace protocol{


bool StreamReader::ReadU8(const char** p, int* left, uint8_t *v){
	int iLen ; 
	const char *cur = *p;
	
	if (left){
		iLen = *left;
	}else{
		iLen = sizeof(uint8_t);
	}
	
	if (static_cast<size_t>(iLen) < sizeof(uint8_t)){
		LOG(ERROR)<<"buffer len: "<<iLen<<"  <  "<<"wanted buffer size: "<<sizeof(uint8_t);
		return false;
	}
	*v = *(reinterpret_cast<const uint8_t*>(cur));
	
	cur		+= sizeof(uint8_t);
	iLen	-= sizeof(uint8_t);

	*p		= cur;
	*left	= iLen;
	return true;
}

bool StreamReader::ReadU16(const char** p, int* left, uint16_t *v){
	int iLen=0; 
	uint16_t value = 0;
	const char *cur = *p;
	if (left){
		iLen = *left;
	}else{
		iLen = sizeof(uint16_t);
	}

	if (static_cast<size_t>(iLen) < sizeof(uint16_t)){
		LOG(ERROR)<<"buffer len: "<<iLen<<"  <  "<<"wanted buffer size: "<<sizeof(uint16_t);
		return false;
	}
	
	value =  *(reinterpret_cast<const uint16_t*>(cur));
	*v = ntohs(value);
	
	cur		+= sizeof(uint16_t);
	iLen	-= sizeof(uint16_t);
	
	*p = cur;
	if (left){
		*left = iLen;
	}
	return true;
}

bool StreamReader::ReadU32(const char** p, int* left, uint32_t *v){
	int iLen=0; 
	uint32_t value = 0;
	const char *cur = *p;
	if (left){
		iLen = *left;
	}else{
		iLen = sizeof(uint32_t);
	}

	if (static_cast<size_t>(iLen) < sizeof(uint32_t)){
		LOG(ERROR)<<"buffer len: "<<iLen<<"  <  "<<"wanted buffer size: "<<sizeof(uint32_t);
		return false;
	}

	value = *(reinterpret_cast<const uint32_t*>(cur));
	*v = ntohl(value);

	cur		+= sizeof(uint32_t);
	iLen	-= sizeof(uint32_t);

	*p = cur;
	if (left){
		*left = iLen;
	}
	return true;
}

bool StreamReader::ReadU64(const char** p, int* left, uint64_t *v){
	int iLen=0; 
	
	const char *cur = *p;
	if (left){
		iLen = *left;
	}else{
		iLen = sizeof(uint64_t);
	}

	if (static_cast<size_t>(iLen) < sizeof(uint64_t)){
		LOG(ERROR)<<"buffer len: "<<iLen<<"  <  "<<"wanted buffer size: "<<sizeof(uint64_t);
		return false;
	}

    uint64_t value = *(reinterpret_cast<const uint64_t*>(cur));
	*v = ntohll(value);

	cur		+= sizeof(uint64_t);
	iLen	-= sizeof(uint64_t);

	*p = cur;
	if (left){
		*left = iLen;
	}
	return true;
}

bool StreamReader::ReadBytes(const char** p, int* left, int len, util::Slice* ret){
	const char* cur = *p;
	int in_len = 0;
	if (left){
		in_len = *left;
	}else{
		in_len = len;
	}

	if (in_len < len){
		LOG(ERROR)<<"buffer len: "<<in_len<<"  <  "<<"wanted buffer size: "<<len;
		return false;
	}

    util::Slice result(cur, len);
	*ret = result;
	
	
	cur		+= len;
	in_len	-= len;

	*p = cur;
	if (left){
		*left = in_len;
	}
	return true;
}



bool StreamWriter::WriteU8(char **p, int* left, uint8_t v){
	int len = sizeof(uint8_t);
	char *cur = *p;
	if (left){
		len = *left;
	}
	if (static_cast<size_t>(len) < sizeof(uint8_t)){
		LOG(ERROR)<<"buffer len: "<<len<<"  <  "<<"wanted buffer size: "<<sizeof(uint8_t);
		return false;
	}
	
	*cur = static_cast<char>(v);
	cur += sizeof(uint8_t);
	len -= sizeof(uint8_t);

	*p = cur;
	if (left){
		*left = len;
	}
	

	return true;
	
}

bool StreamWriter::WriteU16(char **p, int* left, uint16_t v){
	int len = sizeof(uint16_t);
	char *cur = *p;
	if (left){
		len = *left;
	}
	if (static_cast<size_t>(len) < sizeof(uint16_t)){
		LOG(ERROR)<<"buffer len: "<<len<<"  <  "<<"wanted buffer size:"<<sizeof(uint16_t);
		return false;
	}

	*reinterpret_cast<uint16_t*>(cur) = htons(v);


	cur += sizeof(uint16_t);
	len -= sizeof(uint16_t);

	*p = cur;
	if (left){
		*left = len;
	}
	return true;
}

bool StreamWriter::WriteU32(char **p, int* left, uint32_t v){
	int len = sizeof(uint32_t);
	char *cur = *p;
	if (left){
		len = *left;
	}
	if (static_cast<size_t>(len) < sizeof(uint32_t)){
		LOG(ERROR)<<"buffer len: "<<len<<"  <  "<<"wanted buffer size:"<<sizeof(uint32_t);
		return false;
	}

	*reinterpret_cast<uint32_t*>(cur) = htonl(v);

	cur += sizeof(uint32_t);
	len -= sizeof(uint32_t);

	*p = cur;
	if (left){
		*left = len;
	}
	return true;
}

bool StreamWriter::WriteU64(char **p, int* left, uint64_t v){
	int len = sizeof(uint64_t);
	char *cur = *p;
	if (left){
		len = *left;
	}
	if (static_cast<size_t>(len) < sizeof(uint64_t)){
		LOG(ERROR)<<"buffer len: "<<len<<"  <  "<<"wanted buffer size:"<<sizeof(uint64_t);
		return false;
	}


    *reinterpret_cast<uint64_t*>(cur) = htonll(v);
	cur += sizeof(uint64_t);
	len -= sizeof(uint64_t);

	*p = cur;
	if (left){
		*left = len;
	}
	return true;
}

bool StreamWriter::WriteBytes(char** p, int* left, const char *input, int len){
	int len_left = len;
	if (left){
		len_left = *left;
	}

	if (len_left < len){
		LOG(ERROR)<<"buffer len: "<<len_left<<"  <  "<<"wanted buffer size:"<<len;
		return false;
	}

	char* cur = *p;
	memcpy(cur, input, len);
	cur		 += len;
	len_left -= len;

	*p = cur;
	if (left){
		*left = len_left;
	}
	
	return true;
}

}//namespace protocol
}//namespace ldd


