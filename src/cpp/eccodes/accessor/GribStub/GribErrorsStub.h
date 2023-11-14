#pragma once

// Utils to help convert the accessor C code to C++

#include "AccessorDefs.h"
#include "GribCpp/GribStatus.h"
#include <string>
#include <vector>

namespace eccodes::accessor
{

const char* grib_get_error_message(int code);
std::string gribGetErrorMessage(GribStatus code);

}