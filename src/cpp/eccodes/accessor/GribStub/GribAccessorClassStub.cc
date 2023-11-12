#include "GribAccessorClassStub.h"
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

    return GribStatus::NOT_IMPLEMENTED;
}

AccessorPtr gribAccessorFactory(GribActionPtr creator, const long len, AccessorInitData initData)
{
    assert(false); // TO DO

    return nullptr;
}

void gribPushAccessor(AccessorPtr ptr, std::vector<AccessorPtr> ptrList)
{
    assert(false); // TO DO
}

GribStatus gribSectionAdjustSizes(int update, int depth)
{
    assert(false); // TO DO

    return GribStatus::NOT_IMPLEMENTED;
}

}
