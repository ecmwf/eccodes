#pragma once

// Utils to help convert the accessor C code to C++

#include "AccessorDefs.h"
#include "AccessorBuffer.h"
#include "GribCpp/GribStatus.h"
#include <string>
#include <vector>

namespace eccodes::accessor
{

template <typename T> 
GribStatus gribIeeeDecodeArray(AccessorDataPointer buf, size_t nvals, int bytes, std::vector<T> val);
GribStatus gribIeeeEncodeArray(std::vector<double>& val, size_t nvals, int bytes, AccessorDataBuffer& buf);
unsigned long gribIeeeToLong(double x);
unsigned long gribIeee64ToLong(double x);


}