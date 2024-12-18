#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

label="grib_check_message_validity_test"
tempGrib=temp.$label.grib
tempText=temp.$label.txt
tempFilt=temp.$label.filt

sample=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
grib_check_key_equals $sample   isMessageValid 1

# Check regular lat/lon
# ------------------------------
${tools_dir}/grib_set -s Nj=0 $data_dir/sample.grib2 $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "Regular grid Geoiterator" $tempText


# Check reduced Gaussian grid Ni
# ------------------------------
cat >$tempFilt<<EOF
   set Ni = 0; # illegal
   assert ( isMessageValid == 0 );
   set Ni = MISSING;
   assert ( isMessageValid == 1 );
   set Ni = 0; # illegal again
   write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample 2>$tempText
grep -q "Invalid Ni" $tempText
grib_check_key_equals $tempGrib isMessageValid 0
grib_check_key_equals $sample   isMessageValid 1

# Check reduced Gaussian grid pl
# ------------------------------
cat >$tempFilt<<EOF
   meta pl_elem4 element(pl, 4);
   set pl_elem4 = 0;
   assert ( isMessageValid == 0 );
   write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample 2>$tempText
grep -q "Invalid PL array" $tempText
grib_check_key_equals $tempGrib isMessageValid 0


# Check data values
# ------------------------------
${tools_dir}/grib_set -s bitsPerValue=25 $data_dir/sample.grib2 $tempGrib
grib_check_key_equals $tempGrib isMessageValid 0 2>$tempText
grep -q "Data section size mismatch" $tempText


# Clean up
rm -f $tempGrib $tempText $tempFilt
