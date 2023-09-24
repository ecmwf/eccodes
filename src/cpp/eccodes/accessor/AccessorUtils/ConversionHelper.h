#pragma once

// Utils to help convert the accessor C code to C++

#include <string>
#include <cstdarg>
#include <vector>

namespace eccodes::accessor
{

// Create a string directly using format string args (e.g. snprintf)
// For example:
//   C code:    snprintf(result, 128, "+a=%lf +b=%lf", major, minor);
//   C++ code:  std::string result = fmtString("+a=%lf +b=%lf", major, minor);
//
template <typename... Args>
std::string fmtString(const char* format, Args... args) {
    // Determine required size for string (excluding null-terminator)
    size_t size = std::snprintf(nullptr, 0, format, args...) + 1;

    std::vector<char> buf(size);
    std::snprintf(&buf[0], size, format, args...);

    return std::string(&buf[0], &buf[0] + size - 1);  // Exclude null-terminator
}

}