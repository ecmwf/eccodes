#pragma once

// C++ implementation of the functions in grib_accessor_class.cc that are not directly part of the C++ classes

#include "AccessorDefs.h"
#include "GribCpp/GribStatus.h"

namespace eccodes::accessor {

AccessorPtr parentOf(AccessorPtr ptr);

GribStatus gribGetBlockLength(AccessorPtr ptr, size_t& l);

}
