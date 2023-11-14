#include "GribValueStub.h"

#include <cassert>

namespace eccodes::accessor {

GribStatus gribGetDoubleElement(AccessorName const& name, int i, double& val)
{
    assert(false);
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribGetDoubleElementInternal(AccessorName const& name, int i, double& val)
{
    assert(false);
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribGetDoubleElementSet(AccessorName const& name, const std::vector<size_t> indexArray, size_t len, std::vector<double>& valArray)
{
    assert(false);
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribGetDoubleElementSetInternal(AccessorName const& name, const std::vector<size_t> indexArray, size_t len, std::vector<double>& valArray)
{
    assert(false);
    return GribStatus::NOT_IMPLEMENTED;
}

GribStatus gribGetNearestSmallerValue(AccessorName name, double val, double& nearest)
{
    assert(false);
    return GribStatus::NOT_IMPLEMENTED;
}

}
