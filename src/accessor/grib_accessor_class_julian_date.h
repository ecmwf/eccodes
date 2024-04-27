
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_julian_date_h
#define eccodes_accessor_julian_date_h

#include "grib_api_internal.h"
#include "grib_accessor_class_double.h"

class grib_accessor_julian_date_t : public grib_accessor_double_t
{
public:
    /* Members defined in julian_date */
    const char *year;
    const char *month;
    const char *day;
    const char *hour;
    const char *minute;
    const char *second;
    const char *ymd;
    const char *hms;
    char sep[5];
};

class grib_accessor_class_julian_date_t : public grib_accessor_class_double_t
{
public:
    grib_accessor_class_julian_date_t(const char* name) : grib_accessor_class_double_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_julian_date_t{}; }
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int pack_long(grib_accessor*, const long* val, size_t* len) override;
    int pack_string(grib_accessor*, const char*, size_t* len) override;
    int pack_expression(grib_accessor*, grib_expression*) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_long(grib_accessor*, long* val, size_t* len) override;
    int unpack_string(grib_accessor*, char*, size_t* len) override;
    void dump(grib_accessor*, grib_dumper*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};
#endif /* eccodes_accessor_julian_date_h */
