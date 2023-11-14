#include "GribIeeefloatStub.h"

#include <cassert>

namespace eccodes::accessor {

template <> 
GribStatus gribIeeeDecodeArray<float>(AccessorDataPointer buf, size_t nvals, int bytes, std::vector<float> val)
{
    assert(false);
    return GribStatus::NOT_IMPLEMENTED;
}

template <> 
GribStatus gribIeeeDecodeArray<double>(AccessorDataPointer buf, size_t nvals, int bytes, std::vector<double> val)
{
    assert(false);
    return GribStatus::NOT_IMPLEMENTED;
}


GribStatus gribIeeeEncodeArray(std::vector<double>& val, size_t nvals, int bytes, AccessorDataPointer buf)
{
    assert(false);
    return GribStatus::NOT_IMPLEMENTED;
}

unsigned long gribIeeeToLong(double x)
{
    assert(false);
    return 0;
}

unsigned long gribIeee64ToLong(double x)
{
    assert(false);
    return 0;
}

}
