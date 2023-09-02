#include "DoubleData.h"
#include "AccessorFactory.h"

#include "grib_api_internal.h"

namespace eccodes::accessor {

DoubleData::DoubleData(AccessorInitData const& initData)
    : AccessorData(initData)
{
    // TO DO
}

GribType DoubleData::nativeType() const
{
    return GribType::DOUBLE;
}

GribStatus DoubleData::pack(std::vector<double> const& values)
{
    Assert(!values.empty());

    return GribStatus::SUCCESS;
}

GribStatus DoubleData::unpack(std::vector<double> &values) const
{
    std::size_t numDoubles = currentBuffer().size_bytes() / sizeof(double);
    values.resize(numDoubles);

    // Use memcpy() for now...
    memcpy(values.data(), currentBuffer().data(), currentBuffer().size_bytes());

    return GribStatus::SUCCESS;
}


namespace {
AccessorBuilder<DoubleData> DoubleDataBuilder(AccessorType("double"));
}

}
