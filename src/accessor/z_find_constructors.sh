#!/bin/bash

vfuncs=( "dump" "pack_missing" "grib_pack_zero" "is_missing_internal" "pack_double" "pack_float" "pack_expression" "pack_string" "pack_string_array" "pack_long" "pack_bytes" "unpack_bytes" "unpack_double_subarray" "unpack_double" "unpack_float" "unpack_double_element" "unpack_float_element" "unpack_double_element_set" "unpack_float_element_set" "unpack_string" "unpack_string_array" "unpack_long" "get_native_type" "get_next_position_offset" "string_length" "byte_offset" "byte_count" "value_count" "notify_change" "clone" "update_size" "nearest_smaller_value" "preferred_size" "next_accessor" "resize" "compare_accessors" "compare" "add_attribute" "get_attribute_index" "has_attributes" "get_attribute" "init" "post_init" "sub_section" "is_missing" "next_offset" "next" "clear" "make_clone" )


files=( $(find . -name "*.cc") )
#files=( grib_accessor_class_variable.cc grib_accessor_class_check_internal_version.cc )

for f in "${files[@]}"; do
    found=0
    for vf in "${vfuncs[@]}"; do
        output=$(pcregrep -M "_t::grib_accessor_(\n|.)*}" $f | grep -E "\<$vf\>")
        #echo $output
        if [ -n "$output" ]; then
            echo "Found $vf in $f"
            found=1
            break
        fi
    done
done

#Found pack_double in ./grib_accessor_class_codetable.cc
#Found dump in ./grib_accessor_class_gen.cc
#Found value_count in ./grib_accessor_class_ibmfloat.cc
#Found value_count in ./grib_accessor_class_ieeefloat.cc
#Found preferred_size in ./grib_accessor_class_pad.cc
#Found preferred_size in ./grib_accessor_class_padto.cc
#Found preferred_size in ./grib_accessor_class_padtoeven.cc
#Found preferred_size in ./grib_accessor_class_padtomultiple.cc

#Found preferred_size in ./grib_accessor_class_section_padding.cc
#Found value_count in ./grib_accessor_class_signed.cc
#Found value_count in ./grib_accessor_class_unsigned.cc
#Found pack_double in ./grib_accessor_class_variable.cc
