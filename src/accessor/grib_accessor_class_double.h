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

class grib_accessor_double_t : public grib_accessor_gen_t
{
public:
    grib_accessor_double_t() :
        grib_accessor_gen_t() {}
    static inline const AccessorType accessor_type{"double"};
    const AccessorType& getClassName() const override { return accessor_type; }
    long get_native_type() override;
    int pack_missing() override;
    int unpack_string(char*, size_t* len) override;
    void dump(grib_dumper*) override;
    int compare(grib_accessor*) override;
};
