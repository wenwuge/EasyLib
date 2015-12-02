#ifndef LDD_CAGE_INTERNAL_OP_RESULT_H_
#define LDD_CAGE_INTERNAL_OP_RESULT_H_

#include "ldd/cage/op.h"
#include "util.h"

namespace ldd {
namespace cage {

class OpResultBase : public Op::Result {
public:
    OpResultBase() {}
    virtual Status status() const { return Status(rc_); }
    virtual Op::Type type() const = 0;

    virtual const std::string* path_created() const { return NULL; }
    virtual const Stat* stat_set() const { return NULL; }
    virtual void Update(int rc) { rc_ = rc; UpdateBuffer(); }
protected:
    virtual void UpdateBuffer() {};
    int rc_;
};

class OpCreateResult : public OpResultBase {
public:
    OpCreateResult(const std::string& path, Mode::Type mode)
        : OpResultBase(),
          buflen_(MaxCreatedPathLength(path, mode)),
          buffer_(static_cast<char*>(calloc(buflen_, 1))) {}
    virtual ~OpCreateResult() { free(buffer_); }
    virtual Op::Type type() const { return Op::kCreate; }
    virtual const std::string* path_created() const { return &path_created_; }

    virtual void UpdateBuffer() { path_created_.assign(buffer_); }
    char* buffer() const { return buffer_; }
    size_t buflen() const { return buflen_; }
private:
    size_t buflen_;
    char* buffer_;
    std::string path_created_;
};

class OpDeleteResult : public OpResultBase {
public:
    OpDeleteResult() : OpResultBase() {}
    virtual Op::Type type() const { return Op::kDelete; }
};

class OpSetResult : public OpResultBase {
public:
    OpSetResult() : OpResultBase() {}
    virtual Op::Type type() const { return Op::kDelete; }
    virtual const Stat* stat_set() const { return &stat_; }
    virtual void UpdateBuffer() { stat_ = buffer_; }

    ::Stat* buffer() { return &buffer_; }
private:
    ::Stat buffer_;
    Stat stat_;
};

class OpCheckResult : public OpResultBase {
public:
    OpCheckResult() : OpResultBase() {}
    virtual Op::Type type() const { return Op::kCheck; }
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_INTERNAL_OP_RESULT_H_
