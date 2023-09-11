#pragma once

#include "AccessorDefs.h"
#include <string>

// Provides lightweight access an Accessor, for example when using as a member variable

namespace eccodes::accessor {

// Helpers
double toDouble(AccessorName const& name);
long toLong(AccessorName const& name);
std::string toString(AccessorName const& name);
}
