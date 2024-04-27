
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_number_of_points_gaussian_h
#define eccodes_accessor_number_of_points_gaussian_h

#include "grib_api_internal.h"
#include "grib_accessor_class_long.h"

class grib_accessor_number_of_points_gaussian_t : public grib_accessor_long_t
{
public:
    /* Members defined in number_of_points_gaussian */
    const char* ni;
    const char* nj;
    const char* plpresent;
    const char* pl;
    const char* order;
    const char* lat_first;
    const char* lon_first;
    const char* lat_last;
    const char* lon_last;
    const char* support_legacy;
};

class grib_accessor_class_number_of_points_gaussian_t : public grib_accessor_class_long_t
{
public:
    grib_accessor_class_number_of_points_gaussian_t(const char* name) : grib_accessor_class_long_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_number_of_points_gaussian_t{}; }
    int unpack_long(grib_accessor*, long* val, size_t* len) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};
#endif /* eccodes_accessor_number_of_points_gaussian_h */
