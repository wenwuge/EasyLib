#ifndef LDD_CAGE_INTERNAL_RESULT_IMPL_H_
#define LDD_CAGE_INTERNAL_RESULT_IMPL_H_

#include "ldd/cage/result.h"

namespace ldd {
namespace cage {

class ResultImplBase {
public:
    ResultImplBase(int rc) : status_(rc) {}
    virtual ~ResultImplBase() {}

    virtual void Process() = 0;
protected:
    Status status_;
};

template <class ResultT>
class ResultImpl : public ResultImplBase, public ResultT {
public:
    ResultImpl(int rc, const void *ptr)
        : ResultImplBase(rc)
        , callback_((CallbackWrapper::Impl<ResultT>*)ptr) {}
    virtual ~ResultImpl() {}
    virtual void Process() { callback_->ProcessResult(this); }
protected:
    CallbackWrapper::Impl<ResultT> *callback_;
};


class CreateResultImpl : public ResultImpl<CreateResult> {
public:
    CreateResultImpl(int rc, const char *value, const void *ptr)
        : ResultImpl<CreateResult>(rc, ptr)
    {
        if (status_.IsOk()) {
            path_created_.assign(value);
        }
    }
    virtual const Status &status() const { return status_; }
    virtual const std::string &path_created() const { return path_created_; }
private:
    std::string path_created_;
};

class DeleteResultImpl : public ResultImpl<DeleteResult> {
public:
    DeleteResultImpl(int rc, const void *ptr)
        : ResultImpl<DeleteResult>(rc, ptr) {}
    virtual const Status &status() const { return status_; }
};

class ExistsResultImpl : public ResultImpl<ExistsResult> {
public:
    ExistsResultImpl(int rc, const ::Stat *stat, const void *ptr)
        : ResultImpl<ExistsResult>(rc, ptr)
    {
        if (status_.IsOk()) {
            stat_ = *stat;
        }
    }
    virtual const Status &status() const { return status_; }
    virtual const Stat &stat() const { return stat_; }
private:
    Stat stat_;
};

class GetResultImpl : public ResultImpl<GetResult> {
public:
    GetResultImpl(int rc, const char *value, int value_len,
            const ::Stat *stat, const void *ptr)
        : ResultImpl<GetResult>(rc, ptr)
    {
        if (status_.IsOk()) {
            data_.assign(value, value_len);
            stat_ = *stat;
        }
    }
    virtual const Status &status() const { return status_; }
    virtual const std::string &data() const { return data_; }
    virtual const Stat &stat() const { return stat_; }
private:
    std::string data_;
    Stat stat_;
};

class SetResultImpl : public ResultImpl<SetResult> {
public:
    SetResultImpl(int rc, const ::Stat *stat, const void *ptr)
        : ResultImpl<SetResult>(rc, ptr)
    {
        if (status_.IsOk()) {
            stat_ = *stat;
        }
    }
    virtual const Status &status() const { return status_; }
    virtual const Stat &stat() const { return stat_; }
private:
    Stat stat_;
};

class GetAclResultImpl : public ResultImpl<GetAclResult> {
public:
    GetAclResultImpl(int rc, const ::ACL_vector* acls, const ::Stat* stat,
            const void* ptr)
        : ResultImpl<GetAclResult>(rc, ptr)
    {
        if (status_.IsOk()) {
            std::copy(acls->data, acls->data + acls->count,
                    std::back_inserter(acls_));
            stat_ = *stat;
        }
    }
    virtual const Status& status() const { return status_; }
    virtual const std::vector<Acl>& acls() const { return acls_; }
    virtual const Stat& stat() const { return stat_; }
private:
    std::vector<Acl> acls_;
    Stat stat_;
};

class SetAclResultImpl : public ResultImpl<SetAclResult> {
public:
    SetAclResultImpl(int rc, const void* ptr)
        : ResultImpl<SetAclResult>(rc, ptr) { }
    virtual const Status& status() const { return status_; }
};

class GetChildrenResultImpl : public ResultImpl<GetChildrenResult> {
public:
    GetChildrenResultImpl(int rc, const String_vector *strings, const void *ptr)
        : ResultImpl<GetChildrenResult>(rc, ptr)
    {
        if (status_.IsOk()) {
            for (int32_t i = 0; i < strings->count; ++i) {
                children_.push_back(strings->data[i]);
            }
        }
    }
    virtual const Status &status() const { return status_; }
    virtual const std::vector<std::string> &children() const {
        return children_;
    }
private:
    std::vector<std::string> children_;
};

class GetChildrenWithStatResultImpl 
        : public ResultImpl<GetChildrenWithStatResult> {
public:
    GetChildrenWithStatResultImpl(
            int rc, const String_vector *strings, const ::Stat *stat,
            const void *ptr)
        : ResultImpl<GetChildrenWithStatResult>(rc, ptr)
    {
        if (status_.IsOk()) {
            for (int32_t i = 0; i < strings->count; ++i) {
                children_.push_back(strings->data[i]);
            }
            stat_ = *stat;
        }
    }
    virtual const Status &status() const { return status_; }
    virtual const std::vector<std::string> &children() const {return children_;}
    virtual const Stat &stat() const { return stat_; }
private:
    std::vector<std::string> children_;
    Stat stat_;
};

class AddAuthResultImpl : public ResultImpl<AddAuthResult> {
public:
    AddAuthResultImpl(int rc, const void *ptr)
        : ResultImpl<AddAuthResult>(rc, ptr) {}
    virtual const Status &status() const { return status_; }
};

class MultiResultImpl : public ResultImpl<MultiResult> {
public:
    MultiResultImpl(int rc, const void* ptr)
        : ResultImpl<MultiResult>(rc, ptr) {}
    virtual const Status& status() const { return status_; }
    virtual const std::vector<Op::Result*>& results() const {
        return callback_->results(); 
    }
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_INTERNAL_RESULT_IMPL_H_
