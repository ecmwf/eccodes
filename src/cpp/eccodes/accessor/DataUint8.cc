#include "DataUint8.h"
#include "AccessorMaker.h"

#include "grib_api_internal.h"

namespace eccodes::accessor {

DataUint8::DataUint8(const long len, grib_arguments* arg)
    : AccessorData(len, arg)
{
    // TO DO
}

int DataUint8::nativeTypeImpl() const
{
    return GRIB_TYPE_LONG;
}

int DataUint8::packImpl(std::vector<long> const& values)
{
    Assert(!values.empty());

    return GRIB_SUCCESS;
}

int DataUint8::unpackImpl(std::vector<long> &values) const
{
    values.clear();
    values.push_back(42);

    return GRIB_SUCCESS;
}

static AccessorMaker uint8Maker("uint8", AccessorDataType<DataUint8>{});

}
