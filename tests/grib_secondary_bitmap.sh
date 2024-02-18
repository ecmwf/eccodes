#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh

label="grib_secondary_bitmap_test"
tempGribA=temp.$label.A.grib
tempGribB=temp.$label.B.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempRef=temp.$label.ref

# Use a GRIB2 input with a bitmap
# ----------------------------------
input=$data_dir/reduced_latlon_surface.grib2
grib_check_key_equals $input bitmapPresent 1
${tools_dir}/grib_set -s packingType=grid_simple_matrix,NC=1,NR=1 $input $tempGribA
${tools_dir}/grib_set -s matrixBitmapsPresent=1 $tempGribA $tempGribB
${tools_dir}/grib_dump -O $tempGribB
${tools_dir}/grib_dump -Dat $tempGribB

# Call pack_double and unpack_double
# ----------------------------------
cat >$tempFilt<<EOF
  set values = {0, 1, 9999};
  print "[bitmap]";
EOF
${tools_dir}/grib_filter $tempFilt $tempGribB

# data_g1secondary_bitmap
# ----------------------------------
cat >$tempFilt<<EOF
  transient expandBy = 1;
  meta _s data_g1secondary_bitmap(bitmap,secondaryBitmaps,missingValue,expandBy,octetAtWichPackedDataBegins);
  print "[_s]";
  set _s = {0, 9};
EOF
# The above filter is for testing only. Add -f as we know it will fail
${tools_dir}/grib_filter -f $tempFilt $tempGribB

# Clean up
rm -f $tempGribA $tempGribB $tempFilt $tempLog $tempRef
