// ECC-1467

#pragma once

#include "grib_api_internal_cpp.h"
#include <type_traits>
#include <typeinfo>

template <typename T>
class GribAccessorClassGen {
public:
    static int unpack(grib_accessor* a, T* v, size_t* len);
};

template <> int GribAccessorClassGen<long>::unpack(grib_accessor* a, long* v, size_t* len);
template <> int GribAccessorClassGen<char>::unpack(grib_accessor* a, char* v, size_t* len);
template <> int GribAccessorClassGen<float>::unpack(grib_accessor* a, float* v, size_t* len);


template <typename T>
int GribAccessorClassGen<T>::unpack(grib_accessor* a, T* v, size_t* len)
{
    static_assert(std::is_floating_point<T>::value, "Requires floating point numbers");
    int type = GRIB_TYPE_UNDEFINED;
    if (a->cclass->unpack_long && a->cclass->unpack_long != &GribAccessorClassGen<long>::unpack) {
        long val = 0;
        size_t l = 1;
        grib_unpack_long(a, &val, &l);
        *v = val;
        grib_context_log(a->context, GRIB_LOG_DEBUG, "Casting long %s to %s", a->name, typeid(T).name());
        return GRIB_SUCCESS;
    }

    if (a->cclass->unpack_string && a->cclass->unpack_string != &GribAccessorClassGen<char>::unpack) {
        char val[1024];
        size_t l   = sizeof(val);
        char* last = NULL;
        grib_unpack_string(a, val, &l);

        *v = strtod(val, &last);
        if (*last == 0) { /* conversion of string to double worked */
            grib_context_log(a->context, GRIB_LOG_DEBUG, "Casting string %s to long", a->name);
            return GRIB_SUCCESS;
        }
    }

    grib_context_log(a->context, GRIB_LOG_ERROR, "Cannot unpack %s as %s", a->name, typeid(T).name());
    if (grib_get_native_type(grib_handle_of_accessor(a), a->name, &type) == GRIB_SUCCESS) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Hint: Try unpacking as %s", grib_get_type_name(type));
    }

    return GRIB_NOT_IMPLEMENTED;
}
