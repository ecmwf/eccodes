#pragma once

// Utils to help convert the accessor C code to C++

#include "AccessorDefs.h"
#include "GribCpp/GribStatus.h"
#include <string>
#include <vector>

namespace eccodes::accessor
{

GribStatus gribGetSize(AccessorName const& name, size_t& size);

GribStatus gribGetDouble(AccessorName const& name, double& value);
GribStatus gribGetDouble(AccessorName const& name, std::vector<double>& value);
GribStatus gribGetLong(AccessorName const& name, long& value);
GribStatus gribGetLong(AccessorName const& name, std::vector<long>& value);
GribStatus gribGetString(AccessorName const& name, std::string& value);

template<typename T>
GribStatus gribGetArray(AccessorName const& name, std::vector<T>& value)
{
    if constexpr (std::is_floating_point<T>::value) {
        return gribGetDouble(name, value);
    } else {
        return gribGetLong(name, value);
    }
}

GribStatus gribSetDouble(AccessorName const& name, double value);
GribStatus gribSetDouble(AccessorName const& name, std::vector<double> const& values);
GribStatus gribSetLong(AccessorName const& name, long value);
GribStatus gribSetLong(AccessorName const& name, std::vector<long> const& values);
GribStatus gribSetString(AccessorName const& name, std::string value);

template<typename T>
GribStatus gribSetArray(AccessorName const& name, std::vector<T> const& value)
{
    if constexpr (std::is_floating_point<T>::value) {
        return gribSetDouble(name, value);
    } else {
        return gribSetLong(name, value);
    }
}

}