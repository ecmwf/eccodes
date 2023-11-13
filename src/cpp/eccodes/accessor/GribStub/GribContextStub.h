#pragma once

// C++ implementation of the existing grib_util.cc helpers
#include <string>

namespace eccodes::accessor {

std::string gribContextFullDefsPath(std::string basename);

int gribContextGetHandleFileCount();

int gribContextGetHandleTotalCount();

}
