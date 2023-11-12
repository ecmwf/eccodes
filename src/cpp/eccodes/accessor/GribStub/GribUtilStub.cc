#include "GribUtilStub.h"
#include "GribCpp/GribValue.h"
#include <cassert>

namespace eccodes::accessor {

GribStatus gribCheckDataValuesRange(const double minVal, const double maxVal)
{
    assert(false);

    return GribStatus::NOT_IMPLEMENTED;
}

bool gribIsEarthOblate()
{
    long oblate{};
    GribStatus ret = gribGetLong(AccessorName("earthIsOblate"), oblate);

    return (ret == GribStatus::SUCCESS && oblate == 1);
}

bool gribProducingLargeConstantFields(int edition)
{
    assert(false);

    return false;
}

GribStatus gribUtilGribDataQualityCheck(double minVal, double maxVal)
{
    assert(false);

    return GribStatus::NOT_IMPLEMENTED;
}

}
