#!/bin/bash

if [ $# -eq 0 ]; then
  echo "Usage: $0 <filename>"
  exit 1
fi



blacklist=( \
    "grib_accessor_class_gen.cc" \
    "grib_accessor_class_bits.cc" \
    "grib_accessor_class_bitmap.cc" \
    "grib_accessor_class_bufr_elements_table.cc" \
    "grib_accessor_class_bufr_data_array.cc" \
    "grib_accessor_class_bufr_data_element.cc" \
    "grib_accessor_class_unpack_bufr_values.cc" \
    "grib_accessor_class_bytes.cc" \
    "grib_accessor_class_smart_table_column.cc" \
    "grib_accessor_class_bufr_string_values.cc" \
    "grib_accessor_class_codetable_units.cc" \
    "grib_accessor_class_codetable_title.cc" \
    "grib_accessor_class_data_g22order_packing.cc" \
    "grib_accessor_class_g2step_range.cc " \
    "grib_accessor_class_mars_step.cc" \
    "grib_accessor_class_g2level.cc" \
    "grib_accessor_class_g2end_step.cc" \
    "grib_accessor_class_closest_date.cc" \
    "grib_accessor_class_scale.cc" \
    "grib_accessor_class_ibmfloat.cc" \ 
    "grib_accessor_class_ieeefloat.cc" \
    "grib_accessor_class_section_padding.cc" \
    "grib_accessor_class_section_pointer.cc" \
    "grib_accessor_class_signed.cc" \
    "grib_accessor_class_step_in_units.cc" \
    "grib_accessor_class_optimal_step_units.cc" \
    "grib_accessor_class_g1_message_length.cc" \
    "grib_accessor_class_sprintf.cc" \
    "grib_accessor_class_simple_packing_error.cc" \
    "grib_accessor_class_data_simple_packing.cc" \
    "grib_accessor_class_data_sh_packed.cc" \
    "grib_accessor_class_data_sh_unpacked.cc" \
    "grib_accessor_class_data_g1simple_packing.cc" \
    "grib_accessor_class_second_order_bits_per_value.cc" \
    "grib_accessor_class_data_g2simple_packing.cc" \
    "grib_accessor_class_data_g2simple_packing_with_preprocessing.cc" \
    "grib_accessor_class_data_g2complex_packing.cc" \
    "grib_accessor_class_data_g1second_order_row_by_row_packing.cc" \
    "grib_accessor_class_data_g1second_order_constant_width_packing.cc" \
    "grib_accessor_class_data_g1second_order_general_packing.cc" \


)

fns="$@"

for fn in ${fns[@]}; do
  echo "Processing $fn"
  bn=$(basename $fn)
  for bl in ${blacklist[@]}; do
    if [ $bn == $bl ]; then
      echo "Skipping $fn"
      continue 2
    fi
  done

  if [ $bn == "grib_accessor_gen.cc" ]; then
    echo "Skipping $fn"
    continue
  fi

  #sed -i -E "s/#(accessor\/${bn})/\1/g" ../CMakeLists.txt

  #name=$(echo $bn | sed -E 's/\.cc//g')
  #echo "name: $name"
  #name=$(echo $name | sed -E 's/^.{20}//g')
  #echo "name: $name"

  #sed -i -E "s/\/\/(\{ \"${name}\", &grib_accessor_class_${name}, \})/\1/g" ../grib_accessor_factory.h
  #sed -i -E "s/#(${name}, &grib_accessor_class_${name})/\1/g" ../grib_accessor_factory_hash_list
  #sed -i -E "s/\/\/(extern grib_accessor_class\* grib_accessor_class_${name})/\1/g" ../grib_accessor_class.h

  #pushd ..
  #echo $PWD
  #./make_accessor_class_hash.sh
  #popd


  ./transform.py $fn
done

