#pragma once

// Utils to help convert the accessor C code to C++

#include "AccessorDefs.h"
#include "AccessorBuffer.h"
#include "GribCpp/GribStatus.h"
#include <string>
#include <vector>

namespace eccodes::accessor
{

//template <typename T> int grib_ieee_decode_array(grib_context* c, unsigned char* buf, size_t nvals, int bytes, T* val);
template <typename T> 
GribStatus gribIeeeDecodeArray(AccessorDataPointer buf, size_t nvals, int bytes, std::vector<T> val);
//int grib_ieee_encode_array(grib_context* c, double* val, size_t nvals, int bytes, unsigned char* buf);
GribStatus gribIeeeEncodeArray(std::vector<double>& val, size_t nvals, int bytes, AccessorDataPointer buf);
//unsigned long grib_ieee_to_long(double x);
unsigned long gribIeeeToLong(double x);
//unsigned long grib_ieee64_to_long(double x);
unsigned long gribIeee64ToLong(double x);


}