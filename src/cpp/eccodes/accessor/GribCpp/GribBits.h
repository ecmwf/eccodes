#pragma once

// C++ implementation of the functions in grib_accessor_class.cc that are not directly part of the C++ classes

#include "AccessorBuffer.h"
#include <vector>

namespace eccodes::accessor {

void gribSetBitOn(AccessorDataPointer p, long& bitp);
void gribSetBitsOn(AccessorDataPointer p, long& bitp, long nbits);
void gribSetBitOff(AccessorDataPointer p, long& bitp);
void gribSetBit(AccessorDataPointer p, long bitp, int val);


}
