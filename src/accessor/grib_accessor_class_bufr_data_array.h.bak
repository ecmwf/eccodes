
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#ifndef eccodes_accessor_bufr_data_array_h
#define eccodes_accessor_bufr_data_array_h

#include "grib_accessor_class_gen.h"

class grib_accessor_bufr_data_array_t : public grib_accessor_gen_t
{
public:
    const char* bufrDataEncodedName;
    const char* numberOfSubsetsName;
    const char* expandedDescriptorsName;
    const char* flagsName;
    const char* unitsName;
    const char* elementsDescriptorsIndexName;
    const char* compressedDataName;
    bufr_descriptors_array* expanded;
    grib_accessor* expandedAccessor;
    int* canBeMissing;
    long numberOfSubsets;
    long compressedData;
    grib_vdarray* numericValues;
    grib_vsarray* stringValues;
    grib_viarray* elementsDescriptorsIndex;
    int do_decode;
    int bitmapStartElementsDescriptorsIndex;
    int bitmapCurrentElementsDescriptorsIndex;
    int bitmapSize;
    int bitmapStart;
    int bitmapCurrent;
    grib_accessors_list* dataAccessors;
    int unpackMode;
    int bitsToEndData;
    grib_section* dataKeys;
    double* inputBitmap;
    int nInputBitmap;
    int iInputBitmap;
    long* inputReplications;
    int nInputReplications;
    int iInputReplications;
    long* inputExtendedReplications;
    int nInputExtendedReplications;
    int iInputExtendedReplications;
    long* inputShortReplications;
    int nInputShortReplications;
    int iInputShortReplications;
    grib_iarray* iss_list;
    grib_trie_with_rank* dataAccessorsTrie;
    grib_sarray* tempStrings;
    grib_vdarray* tempDoubleValues;
    int change_ref_value_operand;
    size_t refValListSize;
    long* refValList;
    long refValIndex;
    bufr_tableb_override* tableb_override;
    int set_to_missing_if_out_of_range;
};

class grib_accessor_class_bufr_data_array_t : public grib_accessor_class_gen_t
{
public:
    grib_accessor_class_bufr_data_array_t(const char* name) : grib_accessor_class_gen_t(name) {}
    grib_accessor* create_empty_accessor() override { return new grib_accessor_bufr_data_array_t{}; }
    int get_native_type(grib_accessor*) override;
    int pack_double(grib_accessor*, const double* val, size_t* len) override;
    int pack_long(grib_accessor*, const long* val, size_t* len) override;
    int unpack_double(grib_accessor*, double* val, size_t* len) override;
    long byte_count(grib_accessor*) override;
    long byte_offset(grib_accessor*) override;
    long next_offset(grib_accessor*) override;
    int value_count(grib_accessor*, long*) override;
    void destroy(grib_context*, grib_accessor*) override;
    void dump(grib_accessor*, grib_dumper*) override;
    void init(grib_accessor*, const long, grib_arguments*) override;
};

grib_vsarray* accessor_bufr_data_array_get_stringValues(grib_accessor* a);
grib_accessors_list* accessor_bufr_data_array_get_dataAccessors(grib_accessor* a);
grib_trie_with_rank* accessor_bufr_data_array_get_dataAccessorsTrie(grib_accessor* a);
void accessor_bufr_data_array_set_unpackMode(grib_accessor* a, int unpackMode);

#endif /* eccodes_accessor_bufr_data_array_h */
