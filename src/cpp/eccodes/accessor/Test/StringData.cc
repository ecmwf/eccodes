#include "StringData.h"
#include "AccessorFactory.h"

#include "grib_api_internal.h"

namespace eccodes::accessor {

StringData::StringData(AccessorInitData const& initData)
    : AccessorData(initData)
{
    // TO DO
}

GribType StringData::nativeType() const
{
    return GribType::STRING;
}

GribStatus StringData::pack(std::vector<char> const& values)
{
    Assert(!values.empty());

    return GribStatus::SUCCESS;
}

GribStatus StringData::unpack(std::vector<char> &values) const
{
    values.clear();

    values.insert(values.end(), currentBuffer().data(), currentBuffer().data() + currentBuffer().size_bytes());
 
    return GribStatus::SUCCESS;
}


namespace {
AccessorBuilder<StringData> StringDataBuilder(AccessorType("string"));
}

}
