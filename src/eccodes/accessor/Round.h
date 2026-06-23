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

#include "Evaluate.h"

namespace eccodes::accessor
{

class Round : public Evaluate
{
public:
    Round() :
        Evaluate() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    int unpack_double(double* val, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    void init(const long, grib_arguments*) override;
    void destroy(grib_context*) override;
    void dump(eccodes::Dumper*) override;

    static inline const AccessorType accessor_type_{"round"};
};

}  // namespace eccodes::accessor
