#pragma once

#include "GribCpp/GribType.h"
#include <string>
#include <memory>

namespace eccodes::accessor {

struct GribVirtualValue
{
    long lval;
    double dval;
    std::string cval;
    int missing;
    int length;
    GribType type;
};

using GribVirtualValuePtr = std::shared_ptr<GribVirtualValue>;

}
