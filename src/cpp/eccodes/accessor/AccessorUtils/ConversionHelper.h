#pragma once

// Utils to help convert the accessor C code to C++

#include "AccessorDefs.h"
#include "GribStatus.h"
#include <string>
#include <cstdarg>
#include <vector>

namespace eccodes::accessor
{

GribStatus unpackDouble(AccessorName const& name, double& value);
GribStatus unpackLong(AccessorName const& name, long& value);
GribStatus unpackString(AccessorName const& name, std::string& value);

// Create a string directly using format string args (e.g. snprintf)
// For example:
//   C code:    snprintf(result, 128, "+a=%lf +b=%lf", major, minor);
//   C++ code:  std::string result = fmtString(128, "+a=%lf +b=%lf", major, minor);
//
template <typename... Args>
std::string fmtString(const char* format, Args... args) {
    // Determine buffer size
    size_t formatSize = snprintf(nullptr, 0, format, args...);

    char buf[formatSize];
    snprintf(buf, formatSize, format, args...);

    return std::string(buf);
}

// Overload for when the format string doesn't contain any format specifiers, 
// to avoid "warning: format not a string literal and no format arguments [-Wformat-security]"
std::string fmtString(const char* format);

}