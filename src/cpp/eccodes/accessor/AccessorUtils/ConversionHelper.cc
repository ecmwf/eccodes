#include "ConversionHelper.h"

namespace eccodes::accessor {

// Overload for when the format string doesn't contain any format specifiers, 
// to avoid "warning: format not a string literal and no format arguments [-Wformat-security]"
std::string fmtString(std::string format) {
    return format;
}

std::vector<long> stringToLong(std::string val, std::string& last, int base)
{
    std::vector<long> result = {0};
    last.clear();
    char* val_end{};
    result[0] = strtol(val.data(), &val_end, base);

    // Handle any errors
    if(val.data() == val_end) {
        last = val;
    }

    return result;
}

std::vector<double> stringToDouble(std::string val, std::string& last)
{
    std::vector<double> result = {0.0};
    last.clear();
    char* val_end{};
    result[0] = strtod(val.data(), &val_end);

    // Handle any errors
    if(val.data() == val_end) {
        last = val;
    }

    return result;
}

}
