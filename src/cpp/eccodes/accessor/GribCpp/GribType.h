#pragma once

#include <string>
#include <string_view>

namespace eccodes::accessor {

enum class GribType{
    UNDEFINED,
    LONG,
    DOUBLE,
    STRING,
    BYTES,
    SECTION,
    LABEL,
    MISSING,

    // Used for conversion routines - keep last!
    ENUMSIZE
};

constexpr int toInt(GribType type) {
    return static_cast<int>(type);
}

std::string toString(GribType type);
GribType toGribType(std::string_view type);

}
