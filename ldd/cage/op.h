#ifndef LDD_CAGE_OP_H_
#define LDD_CAGE_OP_H_

#include <assert.h>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <zookeeper/proto.h>
#include "ldd/cage/status.h"
#include "ldd/cage/acl.h"
#include "ldd/cage/stat.h"
#include "ldd/cage/mode.h"

namespace ldd {
namespace cage {

class OpCreate;
class OpDelete;
class OpSet;
class OpCheck;

class Op : boost::noncopyable {
public:
    enum Type {
        kCreate = ZOO_CREATE_OP,
        kDelete = ZOO_DELETE_OP,
        kSet = ZOO_SETDATA_OP,
        kCheck = ZOO_CHECK_OP,
    };
    class Result;
    virtual ~Op() {}
    
protected:
    friend class OpCreate;
    friend class OpDelete;
    friend class OpSet;
    friend class OpCheck;
    friend class Keeper;
    Op(const std::string& path) : path_(path) {}
    virtual Result* MakeResult(zoo_op_t* zop) const = 0;
    std::string path_;
};

class Op::Result : boost::noncopyable {
public:
    virtual ~Result() {}
    virtual Status status() const = 0;
    virtual Type type() const = 0;
    virtual const std::string* path_created() const = 0;
    virtual const Stat* stat_set() const = 0;
protected:
    virtual void Update(int rc) = 0;
    friend class CallbackWrapper;
};


class OpCreate : public Op {
public:
    OpCreate(const std::string& path, const std::string& data,
        const std::vector<Acl>& acls, Mode::Type mode);
    virtual ~OpCreate();
private:
    friend class Keeper;
    virtual Result* MakeResult(zoo_op_t* zop) const;
    std::string data_;
    ::ACL_vector acls_;
    Mode::Type mode_;
};

class OpDelete : public Op {
public:
    OpDelete(const std::string& path, int32_t version);
private:
    friend class Keeper;
    virtual Result* MakeResult(zoo_op_t* zop) const;
    int32_t version_;
};

class OpSet : public Op {
public:
    OpSet(const std::string& path, const std::string& data, int32_t version);
private:
    friend class Keeper;
    virtual Result* MakeResult(zoo_op_t* zop) const;
    std::string data_;
    int32_t version_;
};

class OpCheck : public Op {
public:
    OpCheck(const std::string& path, int32_t version);
private:
    friend class Keeper;
    virtual Result* MakeResult(zoo_op_t* zop) const;
    int32_t version_;
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_OP_H_
