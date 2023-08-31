#pragma once

#include "AccessorDefs.h"
#include <vector>
#include <mutex>

namespace eccodes::accessor {

using AccessorEntry = std::pair<eccodes::accessor::AccessorName, eccodes::accessor::AccessorPtr>;

class AccessorStore {
    std::vector<AccessorEntry> store_{};
    std::recursive_mutex mutex_;
    AccessorStore() {}

public:
    static AccessorStore& instance();

    bool addAccessor(AccessorPtr accessor);
    AccessorPtr getAccessor(AccessorName const& name);
    bool destroyAccessor(AccessorName const& name);
};

}
