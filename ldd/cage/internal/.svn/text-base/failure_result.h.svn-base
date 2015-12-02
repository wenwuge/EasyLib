// failure_result.h (2013-09-11)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_CAGE_INTERNAL_FAILURE_RESULT_H_
#define LDD_CAGE_INTERNAL_FAILURE_RESULT_H_

namespace ldd {
namespace cage {

template <class R>
struct FailureResult;

template <>
struct FailureResult<AddAuthResult> : public AddAuthResult {
    FailureResult(const Status& status) : status_(status) {}
    const Status& status() const { return status_; }
private:
    Status status_;
};

template <>
struct FailureResult<CreateResult> : public CreateResult {
    FailureResult(const Status& status): status_(status) {}
    const Status& status() const { return status_; }
    const std::string& path_created() const { return path_created_; }
private:
    Status status_;
    std::string path_created_;
};

template <>
struct FailureResult<DeleteResult> : public DeleteResult {
    FailureResult(const Status& status) : status_(status) {}
    const Status& status() const { return status_; }
private:
    Status status_;
};

template <>
struct FailureResult<ExistsResult> : public ExistsResult {
    FailureResult(const Status& status) : status_(status) {}
    const Status& status() const { return status_; }
    const Stat& stat() const { return stat_; }
private:
    Status status_;
    Stat stat_;
};

template <>
struct FailureResult<GetResult> : public GetResult {
    FailureResult(const Status& status) : status_(status) {}
    const Status& status() const { return status_; }
    const std::string& data() const { return data_; }
    const Stat& stat() const { return stat_; }
private:
    Status status_;
    std::string data_;
    Stat stat_;
};

template <>
struct FailureResult<SetResult> : public SetResult {
    FailureResult(const Status& status) : status_(status) {}
    const Status& status() const { return status_; }
    const Stat& stat() const { return stat_; }
private:
    Status status_;
    Stat stat_;
};

template <>
struct FailureResult<GetAclResult> : public GetAclResult {
    FailureResult(const Status& status) : status_(status) {}
    const Status& status() const { return status_; }
    const std::vector<Acl>& acls() const { return acls_; }
    const Stat& stat() const { return stat_; }
private:
    Status status_;
    std::vector<Acl> acls_;
    Stat stat_;
};

template <>
struct FailureResult<SetAclResult> : public SetAclResult {
    FailureResult(const Status& status) : status_(status) {}
    const Status& status() const { return status_; }
private:
    Status status_;
};

template <>
struct FailureResult<GetChildrenResult> : public GetChildrenResult {
    FailureResult(const Status& status) : status_(status) {}
    const Status& status() const { return status_; }
    const std::vector<std::string>& children() const { return children_; }
private:
    Status status_;
    std::vector<std::string> children_;
};

template <>
struct FailureResult<GetChildrenWithStatResult>
        : public GetChildrenWithStatResult {
    FailureResult(const Status& status) : status_(status) {}
    const Status& status() const { return status_; }
    const std::vector<std::string>& children() const { return children_; }
    const Stat& stat() const { return stat_; }
private:
    Status status_;
    std::vector<std::string> children_;
    Stat stat_;
};

template <>
struct FailureResult<MultiResult> : public MultiResult {
    FailureResult(const Status& status) : status_(status) {}
    const Status& status() const { return status_; }
    const std::vector<Op::Result*>& results() const { return results_; }
private:
    Status status_;
    std::vector<Op::Result*> results_;
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_INTERNAL_FAILURE_RESULT_H_
