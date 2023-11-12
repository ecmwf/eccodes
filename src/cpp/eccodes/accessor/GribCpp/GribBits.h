#pragma once

// C++ implementation of the functions in grib_accessor_class.cc that are not directly part of the C++ classes

#include "AccessorBuffer.h"
#include <vector>

namespace eccodes::accessor {

void gribSetBitOn(DataPointer p, long& bitp);
void gribSetBitsOn(DataPointer p, long& bitp, long nbits);
void gribSetBitOff(DataPointer p, long& bitp);
void gribSetBit(DataPointer p, long bitp, int val);


}
