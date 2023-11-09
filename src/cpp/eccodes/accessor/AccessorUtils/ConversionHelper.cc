#include "ConversionHelper.h"

namespace eccodes::accessor {

// Overload for when the format string doesn't contain any format specifiers, 
// to avoid "warning: format not a string literal and no format arguments [-Wformat-security]"
std::string fmtString(std::string format) {
    return format;
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

}
