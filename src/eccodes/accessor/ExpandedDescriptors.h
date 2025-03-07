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

typedef struct change_coding_params
{
    int associatedFieldWidth;
    int localDescriptorWidth;
    int extraWidth;
    int extraScale;
    int newStringWidth;
    double referenceFactor;
} change_coding_params;

namespace eccodes::accessor
{

class ExpandedDescriptors : public Long
{
public:
    ExpandedDescriptors() :
        Long() { class_name_ = "expanded_descriptors"; }
    grib_accessor* create_empty_accessor() override { return new ExpandedDescriptors{}; }
    long get_native_type() override;
    int pack_long(const long* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int unpack_string_array(char**, size_t* len) override;
    int value_count(long*) override;
    void destroy(grib_context*) override;
    void init(const long, grib_arguments*) override;

    int grib_accessor_expanded_descriptors_set_do_expand(long do_expand);
    bufr_descriptors_array* grib_accessor_expanded_descriptors_get_expanded(int* err);

private:
    const char* unexpandedDescriptors_ = nullptr;
    const char* sequence_ = nullptr;
    const char* expandedName_ = nullptr;
    const char* tablesAccessorName_ = nullptr;
    bufr_descriptors_array* expanded_ = nullptr;
    int rank_ = 0;
    ExpandedDescriptors* expandedAccessor_ = nullptr;
    int do_expand_ = 0;
    grib_accessor* tablesAccessor_ = nullptr;

    int expand();
    void __expand(bufr_descriptors_array* unexpanded, bufr_descriptors_array* expanded, change_coding_params* ccp, int* err);
    bufr_descriptors_array* do_expand(bufr_descriptors_array* unexpanded, change_coding_params* ccp, int* err);
};

}  // namespace eccodes::accessor
