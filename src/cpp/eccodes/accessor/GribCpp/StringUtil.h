#pragma once

// C++ implementation of the existing string_util.cc

#include "GribCpp/GribStatus.h"
#include <string>

namespace eccodes::accessor {

GribStatus stringToLong(std::string input, long& output, int strict);

}
