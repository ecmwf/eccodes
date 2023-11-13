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

//int grib_encode_unsigned_longb(unsigned char* p, unsigned long val, long* bitp, long nb);
GribStatus gribEncodeUnsignedLongb(AccessorDataPointer p, unsigned long val, long& bitPos, long numBits);

}
