#include "ConversionHelper.h"
#include <cassert>

namespace eccodes::accessor {

// Overload for when the format string doesn't contain any format specifiers, 
// to avoid "warning: format not a string literal and no format arguments [-Wformat-security]"
std::string fmtString(std::string format) {
    return format;
}

// Helper to replace strncpy
void copyString(std::string& dest, std::string src, size_t count)
{
    dest = src.substr(0, count);
}

FILE* fopenFromString(std::string name, std::string mode)
{
    return fopen(name.c_str(), mode.c_str());
}

char* fgetsFromString(std::string str, int count, FILE* stream)
{
    return fgets(str.data(), count, stream);
}

long strToLong(std::string val, std::string& last, int base)
{
    last.clear();
    char* val_end{};
    long result = strtol(val.data(), &val_end, base);

    // Handle any errors
    if(val.data() == val_end) {
        last = val;
    }

    return result;
}

double strToDouble(std::string val, std::string& last)
{
    last.clear();
    char* val_end{};
    double result = strtod(val.data(), &val_end);

    // Handle any errors
    if(val.data() == val_end) {
        last = val;
    }

    return result;
}

long strAtoL(std::string input)
{
    return atol(input.c_str());
}

}
