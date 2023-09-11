#pragma once

#include "AccessorUtils/NamedType.h"
#include <string>
#include <vector>
#include <memory>

#define USE_CPP_ACCESSORS

namespace eccodes::accessor {

class Accessor;
using AccessorPtr = std::shared_ptr<Accessor>;

class AccessorData;
using AccessorDataPtr = std::unique_ptr<AccessorData>;

using AccessorName = NamedType<std::string, struct AccessorNameTag>;
using AccessorType = NamedType<std::string, struct AccessorTypeTag>;
using AccessorNameSpace = NamedType<std::string, struct AccessorNameSpaceTag>;

using StringArray = std::vector<std::string>;

}
