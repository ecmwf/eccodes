#include "GribOptimizeDecimalFactorStub.h"

#include <cassert>

namespace eccodes::accessor {

GribStatus gribOptimizeDecimalFactor(AccessorName referenceValue,
                                 const double pmax, const double pmin, const int knbit,
                                 const int compatGribex, const int compat_32bit,
                                 long& kdec, long& kbin, double& ref)
{
    assert(false);
    return GribStatus::NOT_IMPLEMENTED;
}

}
