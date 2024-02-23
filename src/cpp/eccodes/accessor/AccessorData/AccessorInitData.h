#pragma once

#include <string>
#include <variant>
#include <vector>
#include <memory>

struct grib_section;
struct grib_arguments;

// Helper to convert grib_arguments to AccessorInitData

namespace eccodes::accessor {

class GribExpression;
using GribExpressionPtr = std::shared_ptr<GribExpression>;

using AccessorInitArgumentType = std::variant<long, float, double, std::string, GribExpressionPtr>;
using AccessorInitArgumentEntry = std::pair<std::string, AccessorInitArgumentType>;
using AccessorInitArguments = std::vector<AccessorInitArgumentEntry>;

struct AccessorInitData{
    long length{};
    AccessorInitArguments args;
};

AccessorInitData makeInitData(grib_section* section, long len, grib_arguments* args);

} // namespace eccodes::accessor
