#pragma once

// Utils to help convert the accessor C code to C++

#include "AccessorDefs.h"
#include "GribCpp/GribStatus.h"
#include <string>
#include <vector>

namespace eccodes::accessor
{

GribStatus gribGetSize(AccessorName const& name, size_t& size);

GribStatus gribGetFloat(AccessorName const& name, float& value);
GribStatus gribGetFloat(AccessorName const& name, std::vector<float>& value);
GribStatus gribGetDouble(AccessorName const& name, double& value);
GribStatus gribGetDouble(AccessorName const& name, std::vector<double>& value);
GribStatus gribGetLong(AccessorName const& name, long& value);
GribStatus gribGetLong(AccessorName const& name, std::vector<long>& value);
GribStatus gribGetString(AccessorName const& name, std::string& value);

template<typename T>
GribStatus gribGetArray(AccessorName const& name, std::vector<T>& value)
{
    if constexpr (std::is_same<T, float>::value) {
        return gribGetFloat(name, value);
    } 
    else if constexpr (std::is_same<T, double>::value) {
        return gribGetDouble(name, value);
    } 
    else {
        return gribGetLong(name, value);
    }
}

GribStatus gribSetFloat(AccessorName const& name, float value);
GribStatus gribSetFloat(AccessorName const& name, std::vector<float> const& values);
GribStatus gribSetDouble(AccessorName const& name, double value);
GribStatus gribSetDouble(AccessorName const& name, std::vector<double> const& values);
GribStatus gribSetLong(AccessorName const& name, long value);
GribStatus gribSetLong(AccessorName const& name, std::vector<long> const& values);
GribStatus gribSetString(AccessorName const& name, std::string value);

template<typename T>
GribStatus gribSetArray(AccessorName const& name, std::vector<T> const& value)
{
    if constexpr (std::is_same<T, float>::value) {
        return gribSetFloat(name, value);
    }
    else if constexpr (std::is_same<T, double>::value) {
        return gribSetDouble(name, value);
    } 
    else {
        return gribSetLong(name, value);
    }
}

// Stub functions (TO DO)
GribStatus gribGetStringLength(AccessorName const& name, size_t& size);
GribStatus eccGribGetStringLength(AccessorName const& name, size_t& size);

}