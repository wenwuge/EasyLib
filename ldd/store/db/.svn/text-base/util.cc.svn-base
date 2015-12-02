// util.cc (2013-06-25)
// Li Xinjie (xjason.li@gmail.com)

#include <arpa/inet.h>
#include "util.h"

namespace ldd {
namespace storage {

void EncodeRowKey(uint16_t bucket, uint16_t domain, const Slice& rowkey,
        std::string* key) {
    bucket = htons(bucket);
    domain = htons(domain);
    key->assign(reinterpret_cast<const char*>(&bucket), 2);
    key->append(reinterpret_cast<const char*>(&domain), 2);
    key->append(rowkey.data(), rowkey.size());
}
bool DecodeRowKey(const Slice& key, uint16_t* bucket, uint16_t* domain,
        Slice* rowkey) {
    if (key.size() < 4) {
        return false;
    }
    *bucket = *reinterpret_cast<const uint16_t*>(key.data());
    *domain = *reinterpret_cast<const uint16_t*>(key.data() + 2);
    *rowkey = Slice(key.data() + 4, key.size() - 4);
    return true;
}

bool ExtractBucket(const Slice& key, uint16_t* bucket) {
    if (key.size() < 2) {
        return false;
    }
    *bucket = *reinterpret_cast<const uint16_t*>(key.data());
    return true;
}

bool ExtractDomain(const Slice& key, uint16_t* bucket) {
    if (key.size() < 4) {
        return false;
    }
    *bucket = *reinterpret_cast<const uint16_t*>(key.data() + 2);
    return true;
}

Slice ExtractRow(const Slice& key) {
    assert(key.size() >= 4);
    return Slice(key.data() + 4, key.size() - 4);
}

} // namespace storage
} // namespace ldd
