#pragma once

// Utils to help convert the accessor C code to C++

#include "AccessorDefs.h"
#include "GribCpp/GribStatus.h"
#include <string>
#include <cstdarg>
#include <vector>
#include <cassert>

namespace eccodes::accessor
{

// Create a string directly using format string args (e.g. snprintf)
// For example:
//   C code:    snprintf(result, 128, "+a=%lf +b=%lf", major, minor);
//   C++ code:  std::string result = fmtString(128, "+a=%lf +b=%lf", major, minor);
//
template <typename... Args>
std::string fmtString(std::string format, Args... args) {
    // Determine buffer size
    size_t formatSize = snprintf(nullptr, 0, format.c_str(), args...);

    char buf[formatSize];
    snprintf(buf, formatSize, format.c_str(), args...);

    return std::string(buf);
}

// Overload for when the format string doesn't contain any format specifiers, 
// to avoid "warning: format not a string literal and no format arguments [-Wformat-security]"
//std::string fmtString(const char* format);
std::string fmtString(std::string format);

// Helper to replace sscanf with a version that works with std::string and takes args by ref
// which better supports the conversion script!
// Note: Arg 2 [offset] is provided for C code that passes "buf + 2*i" [so offset is 2*i]
//       Pass 0 if not required (i.e. use start of buffer)
template <typename... Args>
int scanString(std::string buffer, size_t offset, std::string format, Args&... args) {
    assert(offset >= buffer.size());

    return sscanf(buffer.data() + offset, format.c_str(), &args...);
}

// Helper to support memcpy from a C buffer into a std::vector
template<typename OUT_TYPE, typename IN_TYPE>
void copyBuffer(std::vector<OUT_TYPE>& out, IN_TYPE* in, size_t numBytes)
{
    assert(sizeof(OUT_TYPE)==sizeof(IN_TYPE));

    size_t numEntries = numBytes/sizeof(IN_TYPE);
    
    std::copy(out.begin(), out.begin() + numEntries, in);
}

// Helper to replace strncpy
void copyString(std::string& dest, std::string src, size_t count);

// Helper to replace codes_fopen
FILE* fopenFromString(std::string name, std::string mode);
char* fgetsFromString(std::string str, int count, FILE* stream );

// Container version of strtoX functions. 
long strToLong(std::string val, std::string& last, int base);
double strToDouble(std::string val, std::string& last);

// std::string version of atol
long strAtoL(std::string input);

// Placeholder for a new version of grib_context_log()
// For now it just swallows all arguments, which allows the convert tool to only
// need to match a single line, even for multi-line arguments
template <typename... Args>
void gribLog(Args... args) 
{

}

}