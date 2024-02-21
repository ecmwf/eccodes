#pragma once

// Utils to help convert the accessor C code to C++

#include "AccessorDefs.h"
#include "GribCpp/GribStatus.h"
#include <string>
#include <vector>

namespace eccodes::accessor
{

GribStatus gribGetDoubleElement(AccessorName const& name, int i, double& val);
GribStatus gribGetDoubleElementInternal(AccessorName const& name, int i, double& val);
GribStatus gribGetDoubleElementSet(AccessorName const& name, const std::vector<size_t> indexArray, size_t len, std::vector<double>& valArray);
GribStatus gribGetDoubleElementSetInternal(AccessorName const& name, const std::vector<size_t> indexArray, size_t len, std::vector<double>& valArray);
GribStatus gribGetNearestSmallerValue(AccessorName name, double val, double& nearest);

GribStatus gribIsMissing(AccessorName name, GribStatus& err);
}