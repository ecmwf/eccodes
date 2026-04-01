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

#include "Long.h"

namespace eccodes::accessor
{

class SpectralTruncation : public Long
{
public:
    SpectralTruncation() :
        Long() { }
    const AccessorType& accessor_type() const override {{ return accessor_type_; }}
    int unpack_long(long* val, size_t* len) override;
    void init(const long, grib_arguments*) override;

private:
    const char* J_ = nullptr;
    const char* K_ = nullptr;
    const char* M_ = nullptr;
    const char* T_ = nullptr;

public:
    static inline const AccessorType accessor_type_{"spectral_truncation"};
};

}  // namespace eccodes::accessor
