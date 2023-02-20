#pragma once

#include "grib_api_internal.h"

template <typename T> int grib_ieee_decode_array(grib_context* c, unsigned char* buf, size_t nvals, int bytes, T* val);
