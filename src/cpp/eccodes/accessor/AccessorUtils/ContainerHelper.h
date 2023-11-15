#pragma once

// Utils to help convert the accessor C code to C++ - container definitions

#include <string>
#include <vector>

namespace eccodes::accessor
{
using IntVectorIterator = std::vector<int>::iterator;
using IntVectorConstIterator = std::vector<int>::const_iterator;

using UnsignedIntVectorIterator = std::vector<unsigned int>::iterator;
using UnsignedIntVectorConstIterator = std::vector<unsigned int>::const_iterator;

using LongVectorIterator = std::vector<long>::iterator;
using LongVectorConstIterator = std::vector<long>::const_iterator;

using UnsignedLongVectorIterator = std::vector<unsigned long>::iterator;
using UnsignedLongVectorConstIterator = std::vector<unsigned long>::const_iterator;

using CharVectorIterator = std::vector<char>::iterator;
using CharVectorConstIterator = std::vector<char>::const_iterator;

using UnsignedCharVectorIterator = std::vector<unsigned char>::iterator;
using UnsignedCharVectorConstIterator = std::vector<unsigned char>::const_iterator;

using FloatVectorIterator = std::vector<float>::iterator;
using FloatVectorConstIterator = std::vector<float>::const_iterator;

using DoubleVectorIterator = std::vector<double>::iterator;
using DoubleVectorConstIterator = std::vector<double>::const_iterator;

}