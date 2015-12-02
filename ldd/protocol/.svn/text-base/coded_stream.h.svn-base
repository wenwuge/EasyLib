
#ifndef __LDD_CODED_STREAM_H_
#define __LDD_CODED_STREAM_H_
#include <stdint.h>
#include "ldd/util/slice.h"




namespace ldd{
namespace protocol{

#if (__BYTE_ORDER == __LITTLE_ENDIAN)
	inline uint64_t htonll(uint64_t v) {
		return \
			((v & 0xff00000000000000ULL) >> 56) |
			((v & 0x00ff000000000000ULL) >> 40) |
			((v & 0x0000ff0000000000ULL) >> 24) |
			((v & 0x000000ff00000000ULL) >> 8) |
			((v & 0x00000000ff000000ULL) << 8) |
			((v & 0x0000000000ff0000ULL) << 24) |
			((v & 0x000000000000ff00ULL) << 40) |
			((v & 0x00000000000000ffULL) << 56);
	}
#else
	inline uint64_t htohll(uint64_t v) {
		return v;
	}
#endif

	inline uint64_t ntohll(uint64_t v) {
		return htonll(v);
	}


class StreamReader{
public:
	static bool  ReadU8 (const char** p, int* left, uint8_t  *v);
	static bool  ReadU16(const char** p, int* left, uint16_t *v);
	static bool  ReadU32(const char** p, int* left, uint32_t *v);
	static bool  ReadU64(const char** p, int* left, uint64_t *v);
	static bool  ReadBytes(const char** p, int* left, int len, util::Slice* ret);
};


class StreamWriter{
public:
	static bool WriteU8( char **p, int* left,  uint8_t v);
	static bool WriteU16(char **p, int* left, uint16_t v);
	static bool WriteU32(char **p, int* left, uint32_t v);
	static bool WriteU64(char **p, int* left, uint64_t v);
	static bool WriteBytes(char** p, int* left, const char *input, int len);
};	


}//namespace protocol
}//namespace ldd


#endif //__LDD_CODED_STREAM_H_
