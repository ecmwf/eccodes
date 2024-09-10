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

class grib_accessor_data_shsimple_packing_t : public grib_accessor_gen_t
{
public:
    grib_accessor_data_shsimple_packing_t() :
        grib_accessor_gen_t() {}
    static inline const AccessorType accessor_type{"data_shsimple_packing"};
    const AccessorType& getClassName() const override { return accessor_type; }
    long get_native_type() override;
    int pack_double(const double* val, size_t* len) override;
    void dump(grib_dumper*) override;
    void init(const long, grib_arguments*) override;

public:
    const char* coded_values_;
    const char* real_part_;
    // TODO(maee): dirty_ shadowed by the derived class
    // int dirty_;
};
