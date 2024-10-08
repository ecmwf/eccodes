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
#include "grib_accessor_class_expanded_descriptors.h"

typedef struct bitmap_s
{
    grib_accessors_list* cursor;
    grib_accessors_list* referredElement;
    grib_accessors_list* referredElementStart;
} bitmap_s;


class grib_accessor_bufr_data_array_t;

typedef int (*codec_element_proc)(grib_context*, grib_accessor_bufr_data_array_t*, int, grib_buffer*, unsigned char*, long*, int, bufr_descriptor*, long, grib_darray*, grib_sarray*);

typedef int (*codec_replication_proc)(grib_context*, grib_accessor_bufr_data_array_t*, int, grib_buffer*, unsigned char*, long*, int, long, grib_darray*, long*);


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

    void accessor_bufr_data_array_set_unpackMode(int);
    grib_accessors_list* accessor_bufr_data_array_get_dataAccessors();
    grib_trie_with_rank* accessor_bufr_data_array_get_dataAccessorsTrie();
    grib_vsarray* accessor_bufr_data_array_get_stringValues();

private:
    const char* bufrDataEncodedName_;
    const char* numberOfSubsetsName_;
    const char* expandedDescriptorsName_;
    const char* flagsName_;
    const char* unitsName_;
    const char* elementsDescriptorsIndexName_;
    const char* compressedDataName_;
    bufr_descriptors_array* expanded_;
    grib_accessor_expanded_descriptors_t* expandedAccessor_;
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

    void restart_bitmap();
    void cancel_bitmap();
    int is_bitmap_start_defined();
    size_t get_length();
    void tableB_override_store_ref_val(grib_context*, int, long);
    int tableB_override_get_ref_val(int, long*);
    void tableB_override_clear(grib_context*);
    int tableB_override_set_key(grib_handle*);
    int get_descriptors();
    int decode_string_array(grib_context*, unsigned char*, long*, bufr_descriptor*);
    int encode_string_array(grib_context*, grib_buffer*, long*, bufr_descriptor*, grib_sarray*);
    int encode_double_array(grib_context*, grib_buffer*, long*, bufr_descriptor*, grib_darray*);
    int encode_double_value(grib_context*, grib_buffer*, long*, bufr_descriptor*, double);
    char* decode_string_value(grib_context*, unsigned char*, long*, bufr_descriptor*, int*);
    double decode_double_value(grib_context*, unsigned char*, long*, bufr_descriptor*, int, int*);
    int encode_new_bitmap(grib_context*, grib_buffer*, long*, int);
    grib_darray* doubleValues = NULL;
    int encode_overridden_reference_value(grib_context*, grib_buffer*, long*, bufr_descriptor*);
    int build_bitmap(unsigned char*, long*, int, grib_iarray*, int);
    int consume_bitmap(int);
    int build_bitmap_new_data(unsigned char*, long*, int, grib_iarray*, int);
    int get_next_bitmap_descriptor_index_new_bitmap(grib_iarray*, int);
    int get_next_bitmap_descriptor_index(grib_iarray*, grib_darray*);
    void push_zero_element(grib_darray*);
    grib_accessor* create_attribute_variable(const char*, grib_section*, int, char*, double, long, unsigned long);
    grib_accessor* create_accessor_from_descriptor(grib_accessor*, grib_section*, long, long, int, int, int, int);
    grib_iarray* set_subset_list( grib_context*, long, long, long, const long*, size_t);
    void print_bitmap_debug_info(grib_context*, bitmap_s*, grib_accessors_list*, int);
    int create_keys(long, long, long);
    void set_input_replications(grib_handle*);
    void set_input_bitmap(grib_handle*);
    int process_elements(int, long, long, long);
    void self_clear();
    grib_darray* decode_double_array(grib_context* c, unsigned char* data, long* pos, bufr_descriptor* bd, int canBeMissing, int*);

    friend int check_end_data(grib_context*, bufr_descriptor*, grib_accessor_bufr_data_array_t*, int);
    friend int decode_element(grib_context*, grib_accessor_bufr_data_array_t*, int, grib_buffer*, unsigned char*, long*, int, bufr_descriptor*, long, grib_darray*, grib_sarray*);
    friend int decode_replication(grib_context*, grib_accessor_bufr_data_array_t*, int, grib_buffer*, unsigned char*, long*, int, long, grib_darray*, long*);
    friend int encode_new_element(grib_context*, grib_accessor_bufr_data_array_t*, int, grib_buffer*, unsigned char*, long*, int, bufr_descriptor*, long, grib_darray*, grib_sarray*);
    friend int encode_new_replication(grib_context*, grib_accessor_bufr_data_array_t*, int, grib_buffer*, unsigned char*, long*, int, long, grib_darray*, long*);
    friend int encode_element(grib_context*, grib_accessor_bufr_data_array_t*, int, grib_buffer*, unsigned char*, long*, int, bufr_descriptor*, long, grib_darray*, grib_sarray*);
    friend int encode_replication(grib_context*, grib_accessor_bufr_data_array_t*, int, grib_buffer*, unsigned char*, long*, int, long, grib_darray*, long*);
};
