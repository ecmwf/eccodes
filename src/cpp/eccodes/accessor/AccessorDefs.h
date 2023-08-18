#ifndef _ACCESSOR_DEFS_H_20230817_
#define _ACCESSOR_DEFS_H_20230817_

#include "NamedType.h"
#include <string>
#include <variant>
#include <vector>
#include <memory>

namespace eccodes::accessor {

class Accessor;
using AccessorPtr = std::shared_ptr<Accessor>;

using AccessorName = NamedType<std::string, struct AccessorNameTag>;
using AccessorType = NamedType<std::string, struct AccessorTypeTag>;
using AccessorNameSpace = NamedType<std::string, struct AccessorNameSpaceTag>;

using AccessorInitDataType = std::variant<long,float,double,std::string>;
using AccessorInitDataEntry = std::pair<std::string, AccessorInitDataType>;
using AccessorInitData = std::vector<AccessorInitDataEntry>;

using StringArray = std::vector<std::string>;

}

#endif // _ACCESSOR_DEFS_H_20230817_
