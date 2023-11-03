#include "GribUtils.h"
#include "GribValue.h"
#include "AccessorUtils/ConversionHelper.h"

namespace eccodes::accessor {

bool gribIsEarthOblate()
{
    long oblate{};
    GribStatus ret = gribGetLong(AccessorName("earthIsOblate"), oblate);

    return (ret == GribStatus::SUCCESS && oblate == 1);
}

}
