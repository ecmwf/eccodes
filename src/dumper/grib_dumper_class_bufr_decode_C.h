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

#include "grib_dumper.h"

namespace eccodes::dumper
{

class BufrDecodeC : public Dumper
{
public:
    BufrDecodeC() { class_name_ = "bufr_decode_C"; }
    int init() override;
    int destroy() override;
    void dump_long(grib_accessor*, const char*) override;
    void dump_bits(grib_accessor*, const char*) override;
    void dump_double(grib_accessor*, const char*) override;
    void dump_string(grib_accessor*, const char*) override;
    void dump_string_array(grib_accessor*, const char*) override;
    void dump_bytes(grib_accessor*, const char*) override;
    void dump_values(grib_accessor*) override;
    void dump_label(grib_accessor*, const char*) override;
    void dump_section(grib_accessor*, grib_block_of_accessors*) override;
    void header(const grib_handle*) override;
    void footer(const grib_handle*) override;


private:
    static inline int depth = 0;

    void dump_attributes(grib_accessor* a, const char* prefix);
    void dump_values_attribute(grib_accessor* a, const char* prefix);
    void dump_long_attribute(grib_accessor* a, const char* prefix);

    long section_offset_    = 0;
    long empty_             = 0;
    //long end_               = 0;
    long isLeaf_            = 0;
    long isAttribute_       = 0;
    grib_string_list* keys_ = nullptr;
};

}  // namespace eccodes::dumper
