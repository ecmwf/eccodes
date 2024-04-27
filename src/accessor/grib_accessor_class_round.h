
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_round_h
#define eccodes_accessor_round_h

#include "grib_api_internal.h"
#include "grib_accessor_class_evaluate.h"

class grib_accessor_round_t : public grib_accessor_evaluate_t
{
public:
    /* Members defined in round */
};

class grib_accessor_class_round_t : public grib_accessor_class_evaluate_t
{
public:
    grib_accessor_class_round_t(const char* name) : grib_accessor_class_evaluate_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_round_t{}; }
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    int unpack_string(grib_accessor*, char*, size_t* len) override;
};
#endif /* eccodes_accessor_round_h */
