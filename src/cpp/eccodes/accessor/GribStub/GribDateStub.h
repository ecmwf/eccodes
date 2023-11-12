#pragma once

// C++ implementation of the existing grib_bits_any_endian.cc

#include "AccessorBuffer.h"

namespace eccodes::accessor {

long gribDateToJulian(long ddate);

int gribDatetimeToJulian(long year, long month, long day,
                         long hour, long minute, long second, double& jd);

int gribDatetimeToJulianD(long year, long month, long day, long hour, long minute,
                          double second, double& jd);

long gribJulianToDate(long jdate);

int gribJulianToDatetime(double jd, long& year, long& month, long& day,
                         long& hour, long& minute, long& second);

}
