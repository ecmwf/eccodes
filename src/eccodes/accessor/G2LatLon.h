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

#include "Double.h"

namespace eccodes::accessor
{

class G2LatLon : public Double
{
public:
    G2LatLon() :
        Double() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    int pack_missing() override;
    int is_missing() override;
    int pack_double(const double* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* grid_ = nullptr;
    int index_ = 0;
    const char* given_ = nullptr;

public:
    static inline const AccessorType accessor_type_{"g2latlon"};
};

}  // namespace eccodes::accessor
