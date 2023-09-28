#include "ConversionHelper.h"
#include "Accessor.h"
#include "AccessorStore.h"
#include "AccessorUtils/AccessorException.h"

// For fallback support whilst building C++ accessors
#include "grib_api_internal.h"

namespace eccodes::accessor {

namespace {
    auto getAccessor = [](AccessorName const& name) -> AccessorPtr {
        if(auto accessorPtr = AccessorStore::instance().getAccessor(name); accessorPtr)
        {
            return accessorPtr;
        }

        //throw AccessorException("AccessorProxy - couldn't get accessor [" + name.get() + ']');
        return nullptr;
    };
}

GribStatus unpackDouble(AccessorName const& name, double& value)
{
//    return getAccessor(name)->unpack<double>();
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        value = accessorPtr->unpack<double>();
        return GribStatus::SUCCESS;
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    int ret = grib_get_double_internal(grib_handle_of_accessor(a), name.get().c_str(), &value);
    return GribStatus{ret};
}

GribStatus unpackLong(AccessorName const& name, long& value)
{
//    return getAccessor(name)->unpack<long>();
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        value = accessorPtr->unpack<long>();
        return GribStatus::SUCCESS;
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    int ret = grib_get_long_internal(grib_handle_of_accessor(a), name.get().c_str(), &value);
    return GribStatus{ret};
}

GribStatus unpackString(AccessorName const& name, std::string& value)
{
//    return getAccessor(name)->unpack<std::string>();
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        value = accessorPtr->unpack<std::string>();
        return GribStatus::SUCCESS;
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    size_t len{512};
    char buffer[len] = {0,};
    int ret = grib_get_string_internal(grib_handle_of_accessor(a), name.get().c_str(), buffer, &len);
    value = buffer;
    return GribStatus{ret};
}

// Overload for when the format string doesn't contain any format specifiers, 
// to avoid "warning: format not a string literal and no format arguments [-Wformat-security]"
std::string fmtString(const char* format) {
    return std::string(format);
}

}
