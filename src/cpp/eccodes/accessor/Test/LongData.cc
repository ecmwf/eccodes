#include "LongData.h"
#include "../AccessorFactory.h"

#include "grib_api_internal.h"

namespace eccodes::accessor {

LongData::LongData(AccessorInitData const& initData)
    : AccessorData(initData)
{
    // TO DO
}

int LongData::nativeType() const
{
    return GRIB_TYPE_LONG;
}

bool LongData::pack(std::vector<long> const& values)
{
    Assert(!values.empty());

    return true;
}

bool LongData::unpack(std::vector<long> &values) const
{
    std::size_t numLongs = currentBuffer().size_bytes() / sizeof(long);
    values.resize(numLongs);

    // Use memcpy() for now...
    memcpy(values.data(), currentBuffer().data(), currentBuffer().size_bytes());

    return true;
}

namespace {
AccessorBuilder<LongData> LongDataBuilder(AccessorType("long"));
}

}
