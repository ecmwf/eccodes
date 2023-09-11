#pragma once

#include "AccessorDefs.h"
#include <vector>
#include <mutex>
#include <map>

struct grib_accessor;

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

    // Allow direct access as this is just for debugging...
    std::map<AccessorName, grib_accessor*> grib_accessors_;
};

// Helper
AccessorPtr get(AccessorName const& name);

// C-Support
void add_grib_accessor(AccessorName const& name, grib_accessor* a);
grib_accessor* get_grib_accessor(AccessorName const& name);

}
