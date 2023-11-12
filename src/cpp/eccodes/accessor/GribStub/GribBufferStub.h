#pragma once

// C++ implementation of the existing grib_bits_any_endian.cc

#include "AccessorDefs.h"
#include "AccessorBuffer.h"

namespace eccodes::accessor {

void griBufferDelete(AccessorDataBuffer b);
void gribBufferReplace(AccessorPtr ptr, AccessorDataBuffer data,
                         size_t newsize, int updateLengths, int updatePaddings);
void gribBufferSetUlengthBits(AccessorDataBuffer b, size_t lengthBits);
AccessorDataBuffer gribCreateGrowableBuffer();

}
