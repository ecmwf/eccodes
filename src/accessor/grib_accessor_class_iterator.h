/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************
 * Enrico Fucile
 *************************/

#pragma once

#include "grib_api_internal.h"
#include "grib_accessor_class_gen.h"

class grib_accessor_iterator_t : public grib_accessor_gen_t
{
public:
    grib_arguments* args;
};

class grib_accessor_class_iterator_t : public grib_accessor_class_gen_t
{
public:
    grib_accessor_class_iterator_t(const char* name) : grib_accessor_class_gen_t(name) {};
    grib_accessor* create_empty_accessor() override { return new grib_accessor_iterator_t{}; }
    void init(grib_accessor* a, const long l, grib_arguments* args) override;
    void dump(grib_accessor* a, grib_dumper* dumper) override;
};


//grib_iterator* grib_iterator_new(const grib_handle* ch, unsigned long flags, int* error)
