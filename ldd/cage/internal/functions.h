#ifndef LDD_CAGE_INTERNAL_FUNCTIONS_H_
#define LDD_CAGE_INTERNAL_FUNCTIONS_H_

#include <zookeeper/zookeeper.h>
#include "ldd/cage/keeper.h"

namespace ldd {
namespace cage {

void WatchSession(zhandle_t *zh, int event, int state, const char *path,
        void *context);
void WatchNode(zhandle_t *zh, int event, int state, const char *path,
        void *context);
void WatchChild(zhandle_t *zh, int event, int state, const char *path,
        void *context);

void CreateCompletion(int rc, const char *name, const void *ptr);
void DeleteCompletion(int rc, const void *ptr);
void ExistsCompletion(int rc, const struct ::Stat *stat,
        const void *ptr);
void GetCompletion(int rc, const char *value, int value_len,
        const struct ::Stat *stat, const void *ptr);
void SetCompletion(int rc, const struct ::Stat *stat,
        const void *ptr);
void GetAclCompletion(int rc, struct ::ACL_vector* acls, 
        struct ::Stat* stat, const void *ptr);
void SetAclCompletion(int rc, const void* ptr);
void GetChildrenCompletion(int rc, const struct String_vector *strings, 
        const void *ptr);
void GetChildrenWithStatCompletion(int rc,
        const struct String_vector *strings, const struct ::Stat *stat,
        const void *ptr);
void AddAuthCompletion(int rc, const void* ptr);
void MultiCompletion(int rc, const void* ptr);

} // naemspace cage
} // namespace ldd

#endif // LDD_CAGE_INTERNAL_FUNCTIONS_H_
