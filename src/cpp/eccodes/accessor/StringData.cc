#include "StringData.h"
#include "AccessorMaker.h"

#include "grib_api_internal.h"

namespace eccodes::accessor {

StringData::StringData(AccessorInitData const& initData)
    : AccessorData(initData)
{
    // TO DO
}

int StringData::nativeType() const
{
    return GRIB_TYPE_STRING;
}

bool StringData::pack(std::vector<char> const& values)
{
    Assert(!values.empty());

    return true;
}

bool StringData::unpack(std::vector<char> &values) const
{
    values.clear();

    values.insert(values.end(), currentBuffer().data(), currentBuffer().data() + currentBuffer().size_bytes());
 
    return true;
}


static AccessorMaker& stringMaker = createMaker<StringData>(AccessorType("string"));

}
