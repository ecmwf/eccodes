#pragma once

#include "AccessorUtils/NamedType.h"
#include <string>
#include <memory>

//namespace eccodes::accessor {

class grib_accessor;
using AccessorPtr = grib_accessor *;
//using AccessorPtr = std::shared_ptr<Accessor>;

class AccessorData;
using AccessorDataPtr = std::unique_ptr<AccessorData>;

using AccessorName = NamedType<std::string, struct AccessorNameTag>;
using AccessorType = NamedType<std::string, struct AccessorTypeTag>;
using AccessorNameSpace = NamedType<std::string, struct AccessorNameSpaceTag>;

//}
