#pragma once

// C++ implementation of the existing grib_util.cc helpers
#include "AccessorDefs.h"
#include "GribCpp/GribStatus.h"

namespace eccodes::accessor {

GribStatus gribRecomposeName(AccessorPtr observer, const std::string uname, std::string fname, int fail);

}
