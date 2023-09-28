#pragma once

// Utils to help convert the accessor C code to C++

#include "AccessorDefs.h"
#include "GribStatus.h"
#include <string>
#include <cstdarg>
#include <vector>

namespace eccodes::accessor
{

GribStatus getSizeHelper(AccessorName const& name, size_t& size);

GribStatus unpackDoubleHelper(AccessorName const& name, double& value);
GribStatus unpackDoubleHelper(AccessorName const& name, std::vector<double>& value);
GribStatus unpackLongHelper(AccessorName const& name, long& value);
GribStatus unpackStringHelper(AccessorName const& name, std::string& value);

GribStatus packDoubleHelper(AccessorName const& name, double value);
GribStatus packLongHelper(AccessorName const& name, long value);
GribStatus packLongHelper(AccessorName const& name, std::vector<long> const& values);
GribStatus packStringHelper(AccessorName const& name, std::string value);

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