#pragma once

#include <string>
#include <variant>
#include <vector>

struct grib_section;
struct grib_arguments;

// Helper to convert grib_arguments to AccessorInitData

namespace eccodes::accessor {

using AccessorInitDataType = std::variant<long,float,double,std::string>;
using AccessorInitDataEntry = std::pair<std::string, AccessorInitDataType>;
using AccessorInitData = std::vector<AccessorInitDataEntry>;

AccessorInitData makeInitData(grib_section* section, grib_arguments* args);

}
