
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

#include "grib_accessor_class_gen.h"

class grib_accessor_bufr_simple_thinning_t : public grib_accessor_gen_t
{
public:
    grib_accessor_bufr_simple_thinning_t() :
        grib_accessor_gen_t() { class_name_ = "bufr_simple_thinning"; }
    grib_accessor* create_empty_accessor() override { return new grib_accessor_bufr_simple_thinning_t{}; }
    long get_native_type() override;
    int pack_long(const long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* doExtractSubsets_;
    const char* numberOfSubsets_;
    const char* extractSubsetList_;
    const char* simpleThinningStart_;
    const char* simpleThinningMissingRadius_;
    const char* simpleThinningSkip_;

    int apply_thinning();
};
