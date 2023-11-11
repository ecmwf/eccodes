#pragma once

// C++ implementation of the existing grib_bits_fast_big_endian.cc

#include "AccessorBuffer.h"
#include "GribCpp/GribStatus.h"

namespace eccodes::accessor {

GribStatus gribEncodeUnsignedLongb(DataPointer p, unsigned long val, long& bitp, long nbits);

}
