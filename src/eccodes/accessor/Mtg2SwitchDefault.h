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

#include "grib_accessor_class_long.h"

class grib_accessor_mtg2_switch_default_t : public grib_accessor_long_t
{
public:
    grib_accessor_mtg2_switch_default_t() :
        grib_accessor_long_t() { class_name_ = "mtg2_switch_default"; }
    grib_accessor* create_empty_accessor() override { return new grib_accessor_mtg2_switch_default_t{}; }

    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* tablesVersion_           = nullptr;
    const char* tablesVersionMTG2Switch_ = nullptr;
    const char* marsClass_               = nullptr;
};
