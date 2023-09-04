#pragma once

#include "AccessorDefs.h"

// Provides lightweight access an Accessor, for example when using as a member variable

namespace eccodes::accessor {

// Helpers
double unpackDouble(AccessorName const& name);
long unpackLong(AccessorName const& name);

}
