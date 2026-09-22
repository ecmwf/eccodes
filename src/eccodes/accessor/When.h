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

namespace eccodes::accessor
{

class When : public Gen
{
public:
    When() :
        Gen() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    long get_native_type() override;
    void dump(eccodes::Dumper*) override;
    void init(const long, grib_arguments*) override;
    int notify_change(grib_accessor* changed) override;

    static inline const AccessorType accessor_type_{"when"};
};

}  // namespace eccodes::accessor
