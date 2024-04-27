#pragma once

#include "grib_api_internal.h"
#include "grib_accessor_class_gen.h"

class grib_accessor_long_t : public grib_accessor_gen_t
{};

class grib_accessor_class_long_t : public grib_accessor_class_gen_t
{
public:
    grib_accessor_class_long_t(const char* name) : grib_accessor_class_gen_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_long_t{}; }
    void init(grib_accessor* a, const long len, grib_arguments* arg) override;
    int get_native_type(grib_accessor* a) override;
    int pack_missing(grib_accessor* a) override;
    int pack_string(grib_accessor* a, const char*, size_t* len) override;
    int unpack_double(grib_accessor* a, double* val, size_t* len) override;
    int unpack_string(grib_accessor* a, char*, size_t* len) override;
    void dump(grib_accessor* a, grib_dumper*) override;
    int compare(grib_accessor* a, grib_accessor*) override;
};
