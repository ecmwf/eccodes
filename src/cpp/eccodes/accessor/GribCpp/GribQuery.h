#pragma once

// C++ implementation of the existing grib_query.cc

#include "AccessorDefs.h"

namespace eccodes::accessor {

AccessorPtr gribFindAccessor(AccessorName const& name);

}
