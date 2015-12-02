// util.h (2013-06-25)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_STORAGE_DB_UTIL_H_
#define LDD_STORAGE_DB_UTIL_H_

#include <stdint.h>
#include "ldd/util/slice.h"

namespace ldd {
namespace storage {

using ::ldd::util::Slice;

void EncodeRowKey(uint16_t bucket, uint16_t domain, const Slice& rowkey,
        std::string* key);
bool DecodeRowKey(const Slice& key, uint16_t* bucket, uint16_t* domain,
        Slice* rowkey);

bool ExtractBucket(const Slice& key, uint16_t* bucket);
bool ExtractDomain(const Slice& key, uint16_t* bucket);
Slice ExtractRow(const Slice& key);

} // namespace storage
} // namespace ldd

#endif // LDD_STORAGE_DB_UTIL_H_
