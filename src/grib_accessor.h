# pragma once

#include "grib_api_internal.h"

template <typename T> int grib_unpack(grib_accessor* a, T* v, size_t* len);
