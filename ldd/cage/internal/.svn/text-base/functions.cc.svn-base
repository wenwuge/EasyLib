#include <boost/foreach.hpp>
#include <glog/logging.h>
#include "ldd/cage/event.h"
#include "util.h"
#include "functions.h"
#include "callback_wrapper.h"
#include "result_impl.h"
#include "keeper_impl.h"

namespace ldd {
namespace cage {

void WatchSession(zhandle_t *zh, int event, int state, const char *path,
        void *context) {
    CHECK_NOTNULL(context);

    CHECK_EQ(event, ZOO_SESSION_EVENT);
    //CHECK_EQ(state, zoo_state(zh));

    Keeper::Impl* zk = static_cast<Keeper::Impl*>(context);
    zk->listener_(state);
}

void WatchNode(zhandle_t *zh, int event, int state, const char *path,
        void *context) {
    CHECK_NOTNULL(context);

    if (event == ZOO_SESSION_EVENT) {
        return;
    }
    CHECK(event == ZOO_CREATED_EVENT || event == ZOO_DELETED_EVENT
            || event == ZOO_CHANGED_EVENT);
    CHECK_NOTNULL(path);

    NodeEvent e(path, static_cast<NodeEvent::Type>(event));

    Keeper::Impl* zk = static_cast<Keeper::Impl*>(context);
    NodeWatchersMap::iterator it = zk->node_watcher_.find(e.path());
    CHECK(it != zk->node_watcher_.end());
    NodeWatchers watchers = it->second;
    CHECK(!watchers.empty());
    zk->node_watcher_.erase(it);
    BOOST_FOREACH(const NodeWatcher& w, watchers) {
        w(e);
    }
}

void WatchChild(zhandle_t *zh, int event, int state, const char *path,
        void *context) {
    CHECK_NOTNULL(context);
    if (event == ZOO_SESSION_EVENT) {
        return;
    }
    CHECK_EQ(event, ZOO_CHILD_EVENT);
    CHECK_NOTNULL(path);

    ChildEvent e(path);

    Keeper::Impl* zk = static_cast<Keeper::Impl*>(context);
    ChildWatchersMap::iterator it = zk->child_watcher_.find(e.path());
    CHECK(it != zk->child_watcher_.end());
    ChildWatchers watchers = it->second;
    CHECK(!watchers.empty());
    zk->child_watcher_.erase(it);
    BOOST_FOREACH(const ChildWatcher& w, watchers) {
        w(e);
    }
}

void CreateCompletion(int rc, const char *name, const void *ptr) {
    CHECK_NOTNULL(ptr);
    CreateResultImpl result(rc, name, ptr);
    result.Process();
}

void DeleteCompletion(int rc, const void *ptr) {
    CHECK_NOTNULL(ptr);
    DeleteResultImpl result(rc, ptr);
    result.Process();
}

void ExistsCompletion(int rc, const struct ::Stat *stat,
        const void *ptr) {
    CHECK_NOTNULL(ptr);
    ExistsResultImpl result(rc, stat, ptr);
    result.Process();
}

void GetCompletion(int rc, const char *value, int value_len,
        const struct ::Stat *stat, const void *ptr) {
    CHECK_NOTNULL(ptr);
    GetResultImpl result(rc, value, value_len, stat, ptr);
    result.Process();
}

void SetCompletion(int rc, const struct ::Stat *stat,
        const void *ptr) {
    CHECK_NOTNULL(ptr);
    SetResultImpl result(rc, stat, ptr);
    result.Process();
}

void GetAclCompletion(int rc, struct ::ACL_vector* acls, 
        struct ::Stat* stat, const void *ptr) {
    CHECK_NOTNULL(ptr);
    GetAclResultImpl result(rc, acls, stat, ptr);
    result.Process();
}

void SetAclCompletion(int rc, const void* ptr) {
    CHECK_NOTNULL(ptr);
    SetAclResultImpl result(rc, ptr);
    result.Process();
}

void GetChildrenCompletion(int rc, const struct String_vector *strings,
        const void *ptr) {
    CHECK_NOTNULL(ptr);
    GetChildrenResultImpl result(rc, strings, ptr);
    result.Process();
}

void GetChildrenWithStatCompletion(int rc,
        const struct String_vector *strings, const struct ::Stat *stat,
        const void *ptr) {
    CHECK_NOTNULL(ptr);
    GetChildrenWithStatResultImpl result(rc, strings, stat, ptr);
    result.Process();
}

void AddAuthCompletion(int rc, const void* ptr) {
    CHECK_NOTNULL(ptr);
    AddAuthResultImpl result(rc, ptr);
    result.Process();
}

void MultiCompletion(int rc, const void* ptr) {
    CHECK_NOTNULL(ptr);
    MultiResultImpl result(rc, ptr);
    result.Process();
}

} // naemspace cage
} // namespace ldd
