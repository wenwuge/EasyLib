#ifndef LDD_CAGE_CALLBACKS_H_
#define LDD_CAGE_CALLBACKS_H_

#include <string>
#include <vector>
#include <boost/function.hpp>
#include "ldd/cage/stat.h"
#include "ldd/cage/status.h"
#include "ldd/cage/result.h"
#include "ldd/cage/event.h"
#include "ldd/cage/keeper_state.h"
#include "ldd/cage/cage_state.h"

namespace ldd {
namespace cage {

typedef boost::function<void(const AddAuthResult&)>
    AddAuthCallback;
typedef boost::function<void(const CreateResult&)>
    CreateCallback;
typedef boost::function<void(const DeleteResult&)>
    DeleteCallback;
typedef boost::function<void(const ExistsResult&)>
    ExistsCallback;
typedef boost::function<void(const GetResult&)>
    GetCallback;
typedef boost::function<void(const SetResult&)>
    SetCallback;
typedef boost::function<void(const GetChildrenResult&)>
    GetChildrenCallback;
typedef boost::function<void(const GetChildrenWithStatResult&)>
    GetChildrenWithStatCallback;
typedef boost::function<void(const GetAclResult&)>
    GetAclCallback;
typedef boost::function<void(const SetAclResult&)>
    SetAclCallback;
typedef boost::function<void(const MultiResult&)>
    MultiCallback;

typedef boost::function<void(const NodeEvent&)>
    NodeWatcher;
typedef boost::function<void(const ChildEvent&)>
    ChildWatcher;

typedef boost::function<void(const KeeperState&)>
    KeeperListener;
typedef boost::function<void(const CageState&)>
    CageListener;

} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_CALLBACKS_H_
