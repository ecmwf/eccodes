#pragma once

// C++ implementation of the functions in grib_accessor_class.cc that are not directly part of the C++ classes

#include <vector>

namespace eccodes::accessor {

void gribSetBitOn(std::vector<unsigned char>& p, long& bitp);
void gribSetBitsOn(std::vector<unsigned char>& p, long& bitp, long nbits);
void gribSetBitOff(std::vector<unsigned char>& p, long& bitp);
void gribSetBit(std::vector<unsigned char>& p, long bitp, int val);


}
