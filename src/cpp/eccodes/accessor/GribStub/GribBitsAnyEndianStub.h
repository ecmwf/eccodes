#pragma once

// C++ implementation of the existing grib_bits_any_endian.cc

#include "AccessorBuffer.h"
#include "GribCpp/GribStatus.h"

namespace eccodes::accessor {

// AccessorDataBuffer& overload added to support conversion from C to C++
unsigned long gribDecodeUnsignedLong(const AccessorDataPointer input, long& bitPos, long numBits);
unsigned long gribDecodeUnsignedLong(const AccessorDataBuffer& input, long& bitPos, long numBits);

// AccessorDataBuffer& overload added to support conversion from C to C++
GribStatus gribEncodeUnsignedLong(AccessorDataPointer p, unsigned long val, long& bitp, long nbits);
GribStatus gribEncodeUnsignedLong(AccessorDataBuffer& p, unsigned long val, long& bitPos, long numBits);

GribStatus gribEncodeUnsignedLongb(AccessorDataPointer p, unsigned long val, long& bitPos, long numBits);

//int grib_decode_long_array(const unsigned char* p, long* bitp, long bitsPerValue, size_t n_vals, long* val);
GribStatus gribDecodeLongArray(const AccessorDataPointer p, long& bitp, long bitsPerValue, size_t nVals, std::vector<long>& val);

}
