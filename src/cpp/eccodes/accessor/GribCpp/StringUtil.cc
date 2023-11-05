#include "StringUtil.h"
#include <charconv>

namespace eccodes::accessor {

GribStatus stringToLong(std::string input, long& output, int strict)
{
    const int base = 10;
    long val{0};
    auto [endptr, ec] = std::from_chars(input.data(), input.data() + input.size(), val, base);

    if (ec == std::errc::invalid_argument ||
        ec == std::errc::result_out_of_range ||
        ec != std::errc())
    {
        return GribStatus::INVALID_ARGUMENT;
    }

    if (strict && endptr != input.data() + input.size())
    {
        // Left over characters - strict has failed!
        return GribStatus::INVALID_ARGUMENT;
    }

    output = val;
    return GribStatus::SUCCESS;
}

}
