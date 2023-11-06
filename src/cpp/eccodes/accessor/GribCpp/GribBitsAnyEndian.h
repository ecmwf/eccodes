#pragma once

// C++ implementation of the existing grib_bits_any_endian.cc

#include "AccessorBuffer.h"

namespace eccodes::accessor {

unsigned long gribDecodeUnsignedLong(AccessorDataBuffer::const_pointer input, long& bitPos, long numBits);

}
