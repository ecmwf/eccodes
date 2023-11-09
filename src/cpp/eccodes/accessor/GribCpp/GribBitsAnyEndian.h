#pragma once

// C++ implementation of the existing grib_bits_any_endian.cc

#include "AccessorBuffer.h"

namespace eccodes::accessor {

unsigned long gribDecodeUnsignedLong(DataPointer input, long& bitPos, long numBits);

}
