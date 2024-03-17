#pragma once

#include "grib_accessor.h"

struct grib_accessors_list
{
public:
    grib_accessors_list();
    ~grib_accessors_list();

    int value_count(size_t* count);
    void push(grib_accessor* a, int rank);
    grib_accessors_list* last();
    int unpack_long(long* val, size_t* buffer_len);
    int unpack_double(double* val, size_t* buffer_len);
    int unpack_float(float* val, size_t* buffer_len);
    int unpack_string(char** val, size_t* buffer_len);

    grib_accessor* accessor;
    int rank;
    grib_accessors_list* next_;
    grib_accessors_list* prev_;
    grib_accessors_list* last_;
};

//grib_accessors_list* grib_accessors_list_create(grib_context* c);
//void grib_accessors_list_delete(grib_context* c, grib_accessors_list* al);

