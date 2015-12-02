// db.cc (2013-06-24)
// Li Xinjie (xjason.li@gmail.com)

#include <glog/logging.h>
#include <ldd/util/mutex.h>
#include <xleveldb/db.h>
#include <xleveldb/comparator.h>
#include <xleveldb/cache.h>
#include <xleveldb/filter_policy.h>
#include "util.h"
#include "db.h"

namespace ldd {
namespace storage {

class XLevelDBIterator : public Iterator {
public:
    XLevelDBIterator(xleveldb::DB::Iterator* db_iter,
            uint16_t bucket, uint16_t domain)
        : db_iter_(db_iter),
        bucket_(bucket),
        domain_(domain) {
        EncodeRowKey(bucket, domain, Slice(), &prefix_);
        db_iter_->Seek(xleveldb::Cell(prefix_, xleveldb::Slice()));
    }
    XLevelDBIterator(xleveldb::DB::Iterator* db_iter,
            uint16_t bucket, uint16_t domain,
            const Slice& rowkey, const Slice& column)
        : db_iter_(db_iter),
        bucket_(bucket),
        domain_(domain) {
        EncodeRowKey(bucket, domain, Slice(), &prefix_);
        std::string x;
        EncodeRowKey(bucket_, domain_, rowkey, &x);
        db_iter_->Seek(xleveldb::Cell(x, xleveldb::Slice(column.data(), column.size())));
    }
    ~XLevelDBIterator() {
        delete db_iter_;
    }
    virtual bool Valid() const {
        if (!db_iter_->Valid()) {
            return false;
        }
        if (!db_iter_->cell().row.starts_with(prefix_)) {
            return false;
        }
        return true;
    }
    virtual void Next() { assert(Valid()); db_iter_->Next(); }
    virtual uint16_t bucket() const { assert(Valid()); return bucket_; }
    virtual uint16_t domain() const { assert(Valid()); return domain_; }
    virtual Slice rowkey() const { assert(Valid()); return ExtractRow(Slice(db_iter_->cell().row.data(), db_iter_->cell().row.size())); }
    virtual Slice column() const { assert(Valid()); return Slice(db_iter_->cell().col.data(), db_iter_->cell().col.size()); }
    virtual Slice value() const { assert(Valid()); return Slice(db_iter_->value().data(), db_iter_->value().size()); }
    virtual uint64_t seq() const { assert(Valid()); return db_iter_->seq(); }
    virtual uint64_t ttl() const { assert(Valid()); return db_iter_->ttl(); }
    virtual Status status() const { return db_iter_->status(); }
private:
    void Init(const Slice& rowkey, const Slice& column) {
    }
    void Init() {
    }
    xleveldb::DB::Iterator* db_iter_;
    uint16_t bucket_;
    uint16_t domain_;
    std::string prefix_;
};

class XLevelDBLimitedIterator: public Iterator {
public:
    XLevelDBLimitedIterator(Iterator* iter, int limit)
        : iter_(iter), limit_(limit) {}
    ~XLevelDBLimitedIterator() {
        delete iter_;
    }
    virtual bool Valid() const {
        return (limit_ > 0 && iter_->Valid());
    }
    virtual void Next() {
        CHECK(Valid());
        if (limit_-- > 0) {
            iter_->Next();
        }
    }
    virtual uint16_t bucket() const {
        CHECK(Valid());
        return iter_->bucket();
    }
    virtual uint16_t domain() const {
        CHECK(Valid());
        return iter_->domain();
    }
    virtual Slice rowkey() const {
        CHECK(Valid());
        return iter_->rowkey();
    }
    virtual Slice column() const {
        CHECK(Valid());
        return iter_->column();
    }
    virtual Slice value() const {
        CHECK(Valid());
        return iter_->value();
    }
    virtual uint64_t seq() const {
        CHECK(Valid());
        return iter_->seq();
    }
    virtual uint64_t ttl() const {
        CHECK(Valid());
        return iter_->ttl();
    }
    virtual Status status() const {
        return iter_->status();
    }
private:
    Iterator* iter_;
    int limit_;
};

class XLevelDBMergingIterator: public Iterator {
public:
    XLevelDBMergingIterator(const std::vector<Iterator*>& list,
            const xleveldb::Comparator* rcmp, const xleveldb::Comparator* ccmp)
        : children_(list),
        current_(NULL),
        rcmp_(rcmp),
        ccmp_(ccmp) {
        FindSmallest();
    }
    ~XLevelDBMergingIterator() {
        std::vector<Iterator*>::iterator it = children_.begin();
        for (; it != children_.end(); ++it) {
            delete *it;
        }
    }
    virtual bool Valid() const {
        return (current_ != NULL);
    }
    virtual void Next() {
        CHECK(Valid());
        current_->Next();
        FindSmallest();
    }
    virtual uint16_t bucket() const {
        CHECK(Valid());
        return current_->bucket();
    }
    virtual uint16_t domain() const {
        CHECK(Valid());
        return current_->domain();
    }
    virtual Slice rowkey() const {
        CHECK(Valid());
        return current_->rowkey();
    }
    virtual Slice column() const {
        CHECK(Valid());
        return current_->column();
    }
    virtual Slice value() const {
        CHECK(Valid());
        return current_->value();
    }
    virtual uint64_t seq() const {
        CHECK(Valid());
        return current_->seq();
    }
    virtual uint64_t ttl() const {
        CHECK(Valid());
        return current_->ttl();
    }
    virtual Status status() const {
        Status status;
        std::vector<Iterator*>::const_iterator it = children_.begin();
        for (; it != children_.end(); ++it) {
            status = (*it)->status();
            if (!status.ok()) {
                break;
            }
        }
        return status;
    }
private:
    void FindSmallest() {
        Iterator* smallest = NULL;
        std::vector<Iterator*>::const_iterator it = children_.begin();
        for (; it != children_.end(); ++it) {
            Iterator* child = *it;
            if (child->Valid()) {
                if (smallest == NULL) {
                    smallest = child;
                } else {
                    int r = rcmp_->Compare(xleveldb::Slice(child->rowkey().data(), child->rowkey().size()),
                            xleveldb::Slice(smallest->rowkey().data(), smallest->rowkey().size()));
                    if (r == 0) {
                        r = ccmp_->Compare(xleveldb::Slice(child->column().data(), child->column().size()),
                                xleveldb::Slice(smallest->column().data(), smallest->column().size()));
                    }
                    if (r < 0) {
                        smallest = child;
                    }
                }
            }
        }
        current_ = smallest;
    }

    std::vector<Iterator*> children_;
    Iterator* current_;
    const xleveldb::Comparator* rcmp_;
    const xleveldb::Comparator* ccmp_;
};

class XLevelDB: public DB {
    friend class XLevelDBIterator;
public:
    XLevelDB(const std::string& path, const std::set<uint16_t>& buckets)
        : leveldb_(NULL),
        rcmp_(xleveldb::BytewiseComparator()),
        ccmp_(xleveldb::BytewiseComparator()),
        path_(path),
        buckets_(buckets)
    {}
    ~XLevelDB() {
        delete leveldb_;
        delete options_.filter_policy;
    }

    Status Init() {
        options_.create_if_missing = true;
        options_.filter_policy = xleveldb::NewBloomFilterPolicy(10);
        options_.max_open_files = 65535;
        options_.block_cache = xleveldb::NewLRUCache(4ULL*1024*1024*1024);
        Status s = xleveldb::DB::Open(options_, path_, &leveldb_);
        if (!s.ok()) {
            return s;
        }
        return s;
    }

    virtual void InsertBucket(uint16_t bucket) {
        util::RWMutexLock l(&mutex_, true);
        buckets_.insert(bucket);
    }
    virtual void DeleteBucket(uint16_t bucket) {
        util::RWMutexLock l(&mutex_, true);
        buckets_.erase(bucket);
    }

    virtual Status Put(uint16_t bucket, uint16_t domain,
            const Slice& rowkey, const Slice& column, 
            const Slice& value, uint64_t* seq, uint64_t ttl) {
        {
            util::RWMutexLock l(&mutex_);
            if (buckets_.find(bucket) == buckets_.end()) {
                return Status::InvalidArgument(xleveldb::Slice());
            }
        }
        std::string key;
        EncodeRowKey(bucket, domain, rowkey, &key);
        return leveldb_->Put(xleveldb::WriteOptions(),
                xleveldb::Cell(key, xleveldb::Slice(column.data(), column.size())),
                xleveldb::Slice(value.data(), value.size()), seq, ttl);
    }
    virtual Status Del(uint16_t bucket, uint16_t domain,
            const Slice& rowkey, const Slice& column,
            uint64_t* seq) {
        {
            util::RWMutexLock l(&mutex_);
            if (buckets_.find(bucket) == buckets_.end()) {
                return Status::InvalidArgument(xleveldb::Slice());
            }
        }
        std::string key;
        EncodeRowKey(bucket, domain, rowkey, &key);
        return leveldb_->Delete(xleveldb::WriteOptions(),
                xleveldb::Cell(key, xleveldb::Slice(column.data(), column.size())),
                seq);
    }
    virtual Status Get(uint16_t bucket, uint16_t domain,
            const Slice& rowkey, const Slice& column,
            std::string* value,
            uint64_t* seq, uint64_t* ttl) {
        {
            util::RWMutexLock l(&mutex_);
            if (buckets_.find(bucket) == buckets_.end()) {
                return Status::InvalidArgument(xleveldb::Slice());
            }
        }
        std::string key;
        EncodeRowKey(bucket, domain, rowkey, &key);
        return leveldb_->Get(xleveldb::ReadOptions(),
                xleveldb::Cell(key, xleveldb::Slice(column.data(), column.size())),
                value, seq, ttl);
    }
    virtual uint64_t LastSequence() {
        return leveldb_->LastSequence();
    }
    virtual Iterator* ListFrom(uint16_t domain,
            const Slice& rowkey, const Slice& column, int limit) {
        util::RWMutexLock l(&mutex_);
        xleveldb::ReadOptions read_options;
        std::vector<Iterator*> list;
        std::set<uint16_t>::const_iterator it = buckets_.begin();
        for (; it != buckets_.end(); ++it) {
            xleveldb::DB::Iterator* db_iter = leveldb_->NewIterator(read_options);
            Iterator* iter = new XLevelDBIterator(db_iter, *it, domain, rowkey, column);
            list.push_back(iter);
        }
        Iterator* iter = new XLevelDBMergingIterator(list, rcmp_, ccmp_);
        return new XLevelDBLimitedIterator(iter, limit);
    }
    virtual Iterator* ListAfter(uint16_t domain,
            const Slice& rowkey, const Slice& column, int limit) {
        util::RWMutexLock l(&mutex_);
        xleveldb::ReadOptions read_options;
        std::vector<Iterator*> list;
        std::set<uint16_t>::const_iterator it = buckets_.begin();
        for (; it != buckets_.end(); ++it) {
            xleveldb::DB::Iterator* db_iter = leveldb_->NewIterator(read_options);
            Iterator* iter = new XLevelDBIterator(db_iter, *it, domain, rowkey, column);
            if (iter->Valid()) {
                Slice r = iter->rowkey();
                Slice c = iter->column();
                if (rcmp_->Compare(xleveldb::Slice(rowkey.data(), rowkey.size()), xleveldb::Slice(r.data(), r.size())) == 0 &&
                    ccmp_->Compare(xleveldb::Slice(column.data(), column.size()), xleveldb::Slice(c.data(), c.size())) == 0) {
                    iter->Next();
                }
            }
            list.push_back(iter);
        }
        Iterator* iter = new XLevelDBMergingIterator(list, rcmp_, ccmp_);
        return new XLevelDBLimitedIterator(iter, limit);
    }
protected:
    xleveldb::DB* leveldb_;
    xleveldb::Options options_;
    const xleveldb::Comparator* rcmp_;
    const xleveldb::Comparator* ccmp_;
    std::string path_;
    util::RWMutex mutex_;
    std::set<uint16_t> buckets_;
};

Status NewXLevelDB(const std::string& path,
        const std::set<uint16_t>& buckets, DB** dbptr) {
    XLevelDB* db = new XLevelDB(path, buckets);
    Status s = db->Init();
    if (!s.ok()) {
        delete db;
        return s;
    }
    *dbptr = db;
    return s;
}

Status NewMemoryDB(const std::string& path,
        const std::set<uint16_t>& buckets, DB** dbptr) {
    Status s;
    return s;
}

} // namespace storage
} // namespace ldd
