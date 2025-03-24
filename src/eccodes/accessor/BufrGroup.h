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

namespace eccodes::accessor
{

class BufrGroup : public Variable
{
public:
    BufrGroup() :
        Variable() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    void dump(eccodes::Dumper*) override;
    grib_accessor* next(grib_accessor*, int explore) override;
private:
    static inline const AccessorType accessor_type_{"bufr_group"};
};

}  // namespace eccodes::accessor
