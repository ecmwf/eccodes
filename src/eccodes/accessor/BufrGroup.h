/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#include "Variable.h"

class grib_accessor_bufr_group_t : public grib_accessor_variable_t
{
public:
    grib_accessor_bufr_group_t() :
        grib_accessor_variable_t() { class_name_ = "bufr_group"; }
    grib_accessor* create_empty_accessor() override { return new grib_accessor_bufr_group_t{}; }
    void dump(eccodes::Dumper*) override;
    grib_accessor* next(grib_accessor*, int explore) override;
};
