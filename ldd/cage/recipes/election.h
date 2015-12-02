// election.h (2013-09-13)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_CAGE_RECIPES_ELECTION_H_
#define LDD_CAGE_RECIPES_ELECTION_H_

#include <stdint.h>
#include <boost/function.hpp>
#include "ldd/cage/cage.h"
#include "ldd/cage/util.h"

namespace ldd {
namespace cage {
namespace recipes {

class Election {
public:
    typedef boost::function<void(bool)> Notifier;
    Election(Cage* cage, const std::string& root, const Notifier& notifier);
    void Start();
    void Stop();
private:
    void CreateRoot();
    void OnCreateRoot(const CreateResult& result);
    void CreateSelf();
    void OnCreateSelf(const CreateResult& result);
    bool GetNodeId(const std::string& node, uint32_t* id);
    bool FindPrevNode(const std::vector<std::string>& nodes,
            std::string* prev);
    void CheckLeader();
    void OnGetChildren(const GetChildrenResult& result);
    void WatchPrev(const NodeEvent& event);
    void OnPrevExists(const ExistsResult& result,
            const std::string& prev);

    Cage* cage_;
    std::string root_;
    Notifier notifier_;
    SequentialNode self_;
};

} // namespace recipes
} // namespace cage
} // namespace ldd

#endif // LDD_CAGE_RECIPES_ELECTION_H_
