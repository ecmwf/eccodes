#include "GribUtils.h"
#include "AccessorUtils/AccessorProxy.h"

namespace eccodes::accessor {

bool gribIsEarthOblate()
{
    long oblate = toLong(AccessorName("earthIsOblate"));

    return (oblate == 1);
}

}
