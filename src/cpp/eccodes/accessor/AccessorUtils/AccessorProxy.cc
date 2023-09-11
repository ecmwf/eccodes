#include "AccessorProxy.h"
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

double toDouble(AccessorName const& name)
{
//    return getAccessor(name)->unpack<double>();
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        return accessorPtr->unpack<double>();
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    double d{};
    grib_get_double_internal(grib_handle_of_accessor(a), name.get().c_str(), &d);
    return d;
}

long toLong(AccessorName const& name)
{
//    return getAccessor(name)->unpack<long>();
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        return accessorPtr->unpack<long>();
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    long l{};
    grib_get_long_internal(grib_handle_of_accessor(a), name.get().c_str(), &l);
    return l;
}

std::string toString(AccessorName const& name)
{
//    return getAccessor(name)->unpack<std::string>();
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        return accessorPtr->unpack<std::string>();
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    size_t len{512};
    char buffer[len] = {0,};
    grib_get_string_internal(grib_handle_of_accessor(a), name.get().c_str(), buffer, &len);
    return {buffer};
}

}
