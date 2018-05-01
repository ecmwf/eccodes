#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

label="grib_ecc-604"

temp_dir=tempdir.${label}
rm -fr $temp_dir
mkdir -p $temp_dir
cd $temp_dir

OUTPUT=output
mkdir -p $OUTPUT
input=$ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl
time ${test_dir}/grib_ecc-604 $input

# Run with forge
# -----------------
# module swap forge/18.0.1
# map ${test_dir}/grib_ecc-604 $input &
#

# Validate results
# -----------------
#num=0
#for ofile in $OUTPUT/output_file_*grib; do
#  ${tools_dir}/grib_compare -H $ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl $ofile
#  num=`expr $num + 1`
#  # If there are too many output files, comparing each one will take a long time!
#  if [ $num -gt 500 ]; then
#    break
#  fi
#done

# Clean up
cd $test_dir
rm -fr $temp_dir
