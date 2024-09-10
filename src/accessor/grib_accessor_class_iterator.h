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

class grib_accessor_iterator_t : public grib_accessor_gen_t
{
public:
    grib_accessor_iterator_t() :
        grib_accessor_gen_t() {}
    static inline const AccessorType accessor_type{"iterator"};
    const AccessorType& getClassName() const override { return accessor_type; }
    void init(const long l, grib_arguments* args) override;
    void dump(grib_dumper* dumper) override;

public:
    grib_arguments* args_;
};

// grib_iterator* grib_iterator_new(const grib_handle* ch, unsigned long flags, int* error)
