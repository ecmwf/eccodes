#include "DateData.h"
#include "AccessorFactory.h"

#include "grib_api_internal.h"

#include <chrono>
#include <sstream>
#include <iomanip>

namespace eccodes::accessor {

DateData::DateData(AccessorInitData const& initData)
    : LongData(initData)
{
    // TO DO
}

GribStatus DateData::pack(std::vector<char> const& values)
{
    Assert(!values.empty());

    return GribStatus::SUCCESS;
}

GribStatus DateData::unpack(std::vector<char> &values) const
{
    std::int64_t timestamp{0};
    memcpy(&timestamp, currentBuffer().data(), sizeof(std::int64_t));

    // Convert int64_t to duration
    std::chrono::seconds duration(timestamp);

    // Convert duration to time_point
    std::chrono::system_clock::time_point tp(duration);

    // For demonstration purposes, let's convert it back to a tm structure and print it:
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&tt), "%F %T");

    std::string timeString = ss.str();

    values.clear();
    values.insert(values.end(), timeString.data(), timeString.data() + timeString.size());

    return GribStatus::SUCCESS;
}

namespace {
AccessorBuilder<DateData> DateDataBuilder(AccessorType("date"));
}

}
