// ECC-1467

#pragma once

#include "grib_api_internal_cpp.h"
#include <type_traits>

template <typename T>
class GribAccessorClassBitmap {
public:
    static int unpack(grib_accessor* a, T* val, size_t* len);
};

template <typename T>
int GribAccessorClassBitmap<T>::unpack(grib_accessor* a, T* val, size_t* len)
{
    static_assert(std::is_floating_point<T>::value, "Requires floating points numbers");
    long pos = a->offset * 8;
    long tlen;
    long i;
    int err           = 0;
    grib_handle* hand = grib_handle_of_accessor(a);

    err = grib_value_count(a, &tlen);
    if (err)
        return err;

    if (*len < tlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Wrong size for %s it contains %ld values", a->name, tlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    for (i = 0; i < tlen; i++) {
        val[i] = (T)grib_decode_unsigned_long(hand->buffer->data, &pos, 1);
    }
    *len = tlen;
    return GRIB_SUCCESS;
}
