#include "GribAccessorClass.h"
#include "Accessor.h"

#include <cassert>

namespace eccodes::accessor {

AccessorPtr parentOf(AccessorPtr ptr)
{
    assert(false); // TO DO

    return nullptr;
}

GribStatus gribGetBlockLength(AccessorPtr ptr, size_t& l)
{
    assert(false); // TO DO

    return GribStatus::SUCCESS;
}


}
