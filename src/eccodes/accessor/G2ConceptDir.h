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

#include "Gen.h"

class grib_accessor_g2_concept_dir_t : public grib_accessor_gen_t
{
public:
    grib_accessor_g2_concept_dir_t() :
        grib_accessor_gen_t() { class_name_ = "g2_concept_dir"; }
    grib_accessor* create_empty_accessor() override { return new grib_accessor_g2_concept_dir_t{}; }
    long get_native_type() override;
    int unpack_string(char*, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* preferLocal_     = nullptr;
    const char* masterDir_       = nullptr;
    const char* localDir_        = nullptr;
    const char* datasetForLocal_ = nullptr;
    int mode_                    = 0;  // 1=conceptsDir1 or 2=conceptsDir2
};
