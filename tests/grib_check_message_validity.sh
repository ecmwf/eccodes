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

cat >$tempFilt<<EOF
   set Ni = 0;
   assert ( isMessageValid == 0 );
   write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample

grib_check_key_equals $tempGrib isMessageValid 0
grib_check_key_equals $sample   isMessageValid 1


cat >$tempFilt<<EOF
   meta pl_elem4 element(pl, 4);
   set pl_elem4 = 0;
   assert ( isMessageValid == 0 );
   write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample

grib_check_key_equals $tempGrib isMessageValid 0

# # Set wrong Nj. Should fail
# input=$samples_dir/reduced_gg_pl_32_grib2.tmpl
# ${tools_dir}/grib_set -s Nj=1 $input $tempGrib
# set +e
# ${tools_dir}/grib_check_gaussian_grid -v $tempGrib 2> $tempText
# status=$?
# set -e
# [ $status -ne 0 ]
# grep -q "Nj is 1 but should be 2\*N" $tempText


# Clean up
rm -f $tempGrib $tempText $tempFilt
