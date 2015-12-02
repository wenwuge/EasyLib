// db.h (2013-06-21)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_STORE_DB_H_
#define LDD_STORE_DB_H_

#include <set>
#include <xleveldb/status.h>
#include "ldd/util/slice.h"

namespace ldd {
namespace storage {

using ::ldd::util::Slice;

using xleveldb::Status;

class Iterator {
public:
    Iterator() {}
    virtual ~Iterator() {}

    virtual bool Valid() const = 0;
    virtual void Next() = 0;
    virtual uint16_t bucket() const = 0;
    virtual uint16_t domain() const = 0;
    virtual Slice rowkey() const = 0;
    virtual Slice column() const = 0;
    virtual Slice value() const = 0;
    virtual uint64_t seq() const = 0;
    virtual uint64_t ttl() const = 0;
    virtual Status status() const = 0;
private:
    Iterator(const Iterator&);
    void operator=(const Iterator&);
};

class DB {
public:
    DB() {}
    virtual ~DB() {}

    virtual void InsertBucket(uint16_t bucket) = 0;
    virtual void DeleteBucket(uint16_t bucket) = 0;

    virtual Status Put(uint16_t bucket, uint16_t domain,
            const Slice& rowkey, const Slice& column, 
            const Slice& value, uint64_t* seq, uint64_t ttl) = 0;
    virtual Status Del(uint16_t bucket, uint16_t domain,
            const Slice& rowkey, const Slice& column,
            uint64_t* seq) = 0;
    virtual Status Get(uint16_t bucket, uint16_t domain,
            const Slice& rowkey, const Slice& column,
            std::string* value,
            uint64_t* seq, uint64_t* ttl) = 0;	// microseconds
    virtual uint64_t LastSequence() = 0;
    virtual Iterator* ListFrom(uint16_t domain,
            const Slice& rowkey, const Slice& column, int limit) = 0;
    virtual Iterator* ListAfter(uint16_t domain,
            const Slice& rowkey, const Slice& column, int limit) = 0;
};

Status NewXLevelDB(const std::string& path,
        const std::set<uint16_t>& buckets, DB** dbptr);

Status NewMemoryDB(const std::string& path,
        const std::set<uint16_t>& buckets, DB** dbptr);

} // namespace store
} // namespace ldd

#endif // LDD_STORE_DB_H_
