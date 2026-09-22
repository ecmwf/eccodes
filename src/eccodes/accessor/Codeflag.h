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

#include "Unsigned.h"

namespace eccodes::accessor
{

class Codeflag : public Unsigned
{
public:
    Codeflag() :
        Unsigned() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    int value_count(long*) override;
    void dump(eccodes::Dumper*) override;
    void init(const long, grib_arguments*) override;

private:
    const char* tablename_ = nullptr;

    int grib_get_codeflag(long code, char* codename);

public:
    static inline const AccessorType accessor_type_{"codeflag"};
};

}  // namespace eccodes::accessor
