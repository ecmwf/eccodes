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
#include "geo/nearest/Nearest.h"

namespace eccodes::accessor
{

class Nearest : public Gen
{
public:
    Nearest() :
        Gen() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    void init(const long l, grib_arguments* args) override;
    void dump(eccodes::Dumper* dumper) override;

private:
    grib_arguments* args_ = nullptr;
    friend eccodes::geo_nearest::Nearest* eccodes::geo_nearest::gribNearestNew(const grib_handle* ch, int* error);
    static inline const AccessorType accessor_type_{"nearest"};
};

}  // namespace eccodes::accessor
