#include "GribType.h"
#include <array>
#include <algorithm>

namespace eccodes::accessor {

using namespace std::string_view_literals;
constexpr std::array<std::string_view, toInt(GribType::ENUMSIZE)> gribTypeStrings = {
    "UNDEFINED"sv,
    "LONG"sv,
    "DOUBLE"sv,
    "STRING"sv,
    "BYTES"sv,
    "SECTION"sv,
    "LABEL"sv,
    "MISSING"sv
};

std::string toString(GribType type)
{
    int index = toInt(type);

    if( index < toInt(GribType::ENUMSIZE)) {
        index = toInt(GribType::UNDEFINED);
    }

    return std::string(gribTypeStrings[index]);
}

GribType toGribType(std::string_view type)
{
    if(auto it = std::find(std::begin(gribTypeStrings), std::end(gribTypeStrings), type); 
        it != std::end(gribTypeStrings))
    {
        return static_cast<GribType>(std::distance(std::begin(gribTypeStrings), it));
    }

    return GribType::UNDEFINED;
}

}
