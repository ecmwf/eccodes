#include "GribValue.h"
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

GribStatus gribGetSize(AccessorName const& name, size_t& size)
{
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        size = accessorPtr->valueCount();
        return GribStatus::SUCCESS;
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    int ret = grib_get_size(grib_handle_of_accessor(a), name.get().c_str(), &size);
    return GribStatus{ret};
}

GribStatus gribGetFloat(AccessorName const& name, float& value)
{
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        value = accessorPtr->unpack<float>();
        return GribStatus::SUCCESS;
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    double dvalue = value;
    int ret = grib_get_double_internal(grib_handle_of_accessor(a), name.get().c_str(), &dvalue);
    return GribStatus{ret};
}

GribStatus gribGetFloat(AccessorName const& name, std::vector<float>& value)
{
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        value = accessorPtr->unpack<std::vector<float>>();
        return GribStatus::SUCCESS;
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    size_t len = value.size();
    int ret = grib_get_float_array_internal(grib_handle_of_accessor(a), name.get().c_str(), value.data(), &len);
    return GribStatus{ret};    
}

GribStatus gribGetDouble(AccessorName const& name, double& value)
{
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

GribStatus gribGetDouble(AccessorName const& name, std::vector<double>& value)
{
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        value = accessorPtr->unpack<std::vector<double>>();
        return GribStatus::SUCCESS;
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    size_t len = value.size();
    int ret = grib_get_double_array_internal(grib_handle_of_accessor(a), name.get().c_str(), value.data(), &len);
    return GribStatus{ret};    
}

GribStatus gribGetLong(AccessorName const& name, long& value)
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

GribStatus gribGetLong(AccessorName const& name, std::vector<long>& value)
{
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        value = accessorPtr->unpack<std::vector<long>>();
        return GribStatus::SUCCESS;
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    size_t len = value.size();
    int ret = grib_get_long_array_internal(grib_handle_of_accessor(a), name.get().c_str(), value.data(), &len);
    return GribStatus{ret};    
}

GribStatus gribGetString(AccessorName const& name, std::string& value)
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

GribStatus gribSetFloat(AccessorName const& name, float value)
{
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        return accessorPtr->pack(value);
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    int ret = grib_set_double_internal(grib_handle_of_accessor(a), name.get().c_str(), value);
    return GribStatus{ret};
}

GribStatus gribSetFloat(AccessorName const& name, std::vector<float> const& values)
{
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        return accessorPtr->pack(values);
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    int ret = grib_set_float_array_internal(grib_handle_of_accessor(a), name.get().c_str(), values.data(), values.size());
    return GribStatus{ret};
}

GribStatus gribSetDouble(AccessorName const& name, double value)
{
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        return accessorPtr->pack(value);
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    int ret = grib_set_double_internal(grib_handle_of_accessor(a), name.get().c_str(), value);
    return GribStatus{ret};
}

GribStatus gribSetDouble(AccessorName const& name, std::vector<double> const& values)
{
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        return accessorPtr->pack(values);
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    int ret = grib_set_double_array_internal(grib_handle_of_accessor(a), name.get().c_str(), values.data(), values.size());
    return GribStatus{ret};
}

GribStatus gribSetLong(AccessorName const& name, long value)
{
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        return accessorPtr->pack(value);
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    int ret = grib_set_long_internal(grib_handle_of_accessor(a), name.get().c_str(), value);
    return GribStatus{ret};
}

GribStatus gribSetLong(AccessorName const& name, std::vector<long> const& values)
{
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        return accessorPtr->pack(values);
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    int ret = grib_set_long_array_internal(grib_handle_of_accessor(a), name.get().c_str(), values.data(), values.size());
    return GribStatus{ret};
}

GribStatus gribSetString(AccessorName const& name, std::string value)
{
    if(auto accessorPtr = getAccessor(name); accessorPtr)
    {
        return accessorPtr->pack(value);
    }

    // C++ Accessor not found - fall back to C (should be safe!)
    grib_accessor* a = get_grib_accessor(name);
    Assert(a);
    size_t len = value.length();
    int ret = grib_set_string_internal(grib_handle_of_accessor(a), name.get().c_str(), value.c_str(), &len);
    return GribStatus{ret};
}

}
