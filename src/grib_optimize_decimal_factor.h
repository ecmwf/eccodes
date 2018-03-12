#ifndef GRIB_OPTIMIZE_DECIMAL_FACTOR_H
#define GRIB_OPTIMIZE_DECIMAL_FACTOR_H

#include "grib_api_internal.h"

int grib_optimize_decimal_factor (grib_accessor * a, const char * reference_value,
                                  const double pmax, const double pmin, const int knbit, 
                                  const int compat_gribex, const int compat_32bit, 
                                  long * kdec, long * kbin, double * ref);

#endif

