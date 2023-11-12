#pragma once

// C++ implementation of the existing grib_util.cc helpers
#include "GribCpp/GribStatus.h"

namespace eccodes::accessor {

GribStatus gribCheckDataValuesRange(const double minVal, const double maxVal);
bool gribIsEarthOblate();
bool gribProducingLargeConstantFields(int edition);
GribStatus gribUtilGribDataQualityCheck(double minVal, double maxVal);

}
