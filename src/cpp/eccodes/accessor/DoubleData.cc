#include "DoubleData.h"
#include "AccessorMaker.h"

#include "grib_api_internal.h"

namespace eccodes::accessor {

DoubleData::DoubleData(AccessorInitData const& initData)
    : AccessorData(initData)
{
    // TO DO
}

int DoubleData::nativeType() const
{
    return GRIB_TYPE_DOUBLE;
}

bool DoubleData::pack(std::vector<double> const& values)
{
    Assert(!values.empty());

    return true;
}

bool DoubleData::unpack(std::vector<double> &values) const
{
    std::size_t numDoubles = currentBuffer().size_bytes() / sizeof(double);
    values.resize(numDoubles);

    // Use memcpy() for now...
    memcpy(values.data(), currentBuffer().data(), currentBuffer().size_bytes());

    return true;
}


static AccessorMaker& doubleMaker = createMaker<DoubleData>(AccessorType("double"));

}
