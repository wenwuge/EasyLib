#ifndef LDD_CAGE_EVENT_H_
#define LDD_CAGE_EVENT_H_

#include <string>
#include <zookeeper/zookeeper.h>

namespace ldd {
namespace cage {

class NodeEvent {
public:
    enum Type {
        kCreated = 1,
        kDeleted = 2,
        kChanged = 3,
    };
    NodeEvent(const std::string& path, Type type) : path_(path), type_(type) {}
    Type type() const { return type_; }
    const std::string& path() const { return path_; }
    bool IsCreated() const { return type() == kCreated; }
    bool IsDeleted() const { return type() == kDeleted; }
    bool IsChanged() const { return type() == kChanged; }
private:
    std::string path_;
    Type type_;
};

class ChildEvent {
public:
    ChildEvent(const std::string& path) : path_(path) {}
    const std::string& path() const { return path_; }
private:
    std::string path_;
};

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_EVENT_H_
