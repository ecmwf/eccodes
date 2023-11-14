#pragma once

// Utils to help convert the accessor C code to C++

#include "AccessorDefs.h"
#include "GribCpp/GribStatus.h"
#include <string>
#include <vector>

namespace eccodes::accessor
{

//long grib_get_binary_scale_fact(double max, double min, long bpval, int* error);
long gribGetBinaryScaleFact(double max, double min, long bpval, GribStatus& error);

}