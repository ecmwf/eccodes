#pragma once

// Utils to help convert the accessor C code to C++

#include "AccessorDefs.h"
#include "AccessorBuffer.h"
#include "GribCpp/GribStatus.h"
#include <string>
#include <vector>

namespace eccodes::accessor
{

GribStatus gribOptimizeDecimalFactor(AccessorName referenceValue,
                                 const double pmax, const double pmin, const int knbit,
                                 const int compatGribex, const int compat32bit,
                                 long& kdec, long& kbin, double& ref);

}