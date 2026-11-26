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

#include "DataSimplePacking.h"

namespace eccodes::accessor
{

class DataG2SimplePacking : public DataSimplePacking
{
public:
    DataG2SimplePacking() :
        DataSimplePacking() {}
    const AccessorType& accessor_type() const override { return accessor_type_; }
    int pack_bytes(const unsigned char*, size_t* len) override;
    int pack_double(const double* val, size_t* len) override;
    int pack_float(const float* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

    static constexpr AccessorType accessor_type_{ "data_g2simple_packing" };

private:
    // Floats are packed as the converted doubles; with the new simple packing the values are not
    // converted (sp::pack of floats gives the codes of the converted doubles)
    template <typename T>
    int pack(const T* val, size_t* len);
    template <typename T>
    int pack_new(const T* val, size_t n_vals, bool& packed);
    int pack_legacy(const double* val, size_t* len);
};

}  // namespace eccodes::accessor
