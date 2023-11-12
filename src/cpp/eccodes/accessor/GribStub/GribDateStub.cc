#include "GribDateStub.h"
#include "Accessor.h"

#include <cassert>

namespace eccodes::accessor {

long gribDateToJulian(long ddate)
{
    assert(false);
    return 0;
}

int gribDatetimeToJulian(long year, long month, long day,
                         long hour, long minute, long second, double& jd)
{
    assert(false);
    return 0;
}

int gribDatetimeToJulianD(long year, long month, long day, long hour, long minute,
                          double second, double& jd)
{
    assert(false);
    return 0;
}

//long grib_julian_to_date(long jdate);
long gribJulianToDate(long jdate)
{
    assert(false);
    return 0;
}

int gribJulianToDatetime(double jd, long& year, long& month, long& day,
                         long& hour, long& minute, long& second)
{
    assert(false);
    return 0;
}


}
