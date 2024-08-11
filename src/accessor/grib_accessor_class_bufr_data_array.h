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

class grib_accessor_bufr_data_array_t : public grib_accessor_gen_t
{
public:
    grib_accessor_bufr_data_array_t() :
        grib_accessor_gen_t() { class_name_ = "bufr_data_array"; }
    grib_accessor* create_empty_accessor() override { return new grib_accessor_bufr_data_array_t{}; }
    long get_native_type() override;
    int pack_double(const double* val, size_t* len) override;
    int pack_long(const long* val, size_t* len) override;
    int unpack_double(double* val, size_t* len) override;
    long byte_count() override;
    long byte_offset() override;
    long next_offset() override;
    int value_count(long*) override;
    void destroy(grib_context*) override;
    void dump(grib_dumper*) override;
    void init(const long, grib_arguments*) override;

public:
    const char* bufrDataEncodedName_;
    const char* numberOfSubsetsName_;
    const char* expandedDescriptorsName_;
    const char* flagsName_;
    const char* unitsName_;
    const char* elementsDescriptorsIndexName_;
    const char* compressedDataName_;
    bufr_descriptors_array* expanded_;
    grib_accessor* expandedAccessor_;
    int* canBeMissing_;
    long numberOfSubsets_;
    long compressedData_;
    grib_vdarray* numericValues_;
    grib_vsarray* stringValues_;
    grib_viarray* elementsDescriptorsIndex_;
    int do_decode_;
    int bitmapStartElementsDescriptorsIndex_;
    int bitmapCurrentElementsDescriptorsIndex_;
    int bitmapSize_;
    int bitmapStart_;
    int bitmapCurrent_;
    grib_accessors_list* dataAccessors_;
    int unpackMode_;
    int bitsToEndData_;
    grib_section* dataKeys_;
    double* inputBitmap_;
    int nInputBitmap_;
    int iInputBitmap_;
    long* inputReplications_;
    int nInputReplications_;
    int iInputReplications_;
    long* inputExtendedReplications_;
    int nInputExtendedReplications_;
    int iInputExtendedReplications_;
    long* inputShortReplications_;
    int nInputShortReplications_;
    int iInputShortReplications_;
    grib_iarray* iss_list_;
    grib_trie_with_rank* dataAccessorsTrie_;
    grib_sarray* tempStrings_;
    grib_vdarray* tempDoubleValues_;
    int change_ref_value_operand_;
    size_t refValListSize_;
    long* refValList_;
    long refValIndex_;
    bufr_tableb_override* tableb_override_;
    int set_to_missing_if_out_of_range_;
};

grib_vsarray* accessor_bufr_data_array_get_stringValues(grib_accessor*);
grib_accessors_list* accessor_bufr_data_array_get_dataAccessors(grib_accessor*);
grib_trie_with_rank* accessor_bufr_data_array_get_dataAccessorsTrie(grib_accessor*);
void accessor_bufr_data_array_set_unpackMode(grib_accessor*, int unpackMode);
