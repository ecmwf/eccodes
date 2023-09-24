#pragma once

#include "AccessorDefs.h"
#include "GribStatus.h"
#include <string>

// Provides lightweight access an Accessor, for example when using as a member variable

namespace eccodes::accessor {

// Helpers
double toDouble(AccessorName const& name);
long toLong(AccessorName const& name);
std::string toString(AccessorName const& name);

// These functions are provided to assist with conversions from C code
GribStatus toDouble(AccessorName const& name, double& value);
GribStatus toLong(AccessorName const& name, long& value);
GribStatus toString(AccessorName const& name, std::string& value);

}
