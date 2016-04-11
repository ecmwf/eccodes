#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

REDIRECT=/dev/null

infile="${data_dir}/regular_latlon_surface.grib1"
outfile=${infile}.compare.$$

rm -f $outfile

${tools_dir}grib_set -s shortName=2d $infile $outfile
${tools_dir}grib_compare -b indicatorOfParameter,paramId,shortName $infile $outfile > $REDIRECT

# Test the -r switch
# ----------------------------------------
infile=${data_dir}/v.grib2
for i in 1 2 3; do
  ${tools_dir}grib_copy -wcount=$i $infile temp_comp.$i
done
cat temp_comp.1 temp_comp.2 temp_comp.3 > temp_comp.123
cat temp_comp.3 temp_comp.2 temp_comp.1 > temp_comp.321

# Compare files in which the messages are not in the same order
${tools_dir}grib_compare -r temp_comp.123 temp_comp.321

rm -f temp_comp.1 temp_comp.2 temp_comp.3 temp_comp.123 temp_comp.321

# GRIB-797: test last argument being a directory
# ----------------------------------------
temp_dir=tempdir.grib_compare
mkdir -p $temp_dir
cp $infile $temp_dir
${tools_dir}grib_compare $infile  $temp_dir
rm -rf $temp_dir

# ECC-245: blacklist and 2nd order packing
# ----------------------------------------
temp1=grib_compare_temp1.grib
temp2=grib_compare_temp2.grib
${tools_dir}grib_copy -w count=25 ${data_dir}/lfpw.grib1 $temp1
${tools_dir}grib_copy -w count=30 ${data_dir}/lfpw.grib1 $temp2

# This should fail but not crash! so check exit code is not 134
set +e
${tools_dir}grib_compare -b firstOrderValues $temp1 $temp2 >/dev/null
status=$?
set -e
[ $status -eq 1 ]

# GRIB-915: blacklisting totalLength key
${tools_dir}grib_copy -w count=1 ${data_dir}/v.grib2 $temp1
${tools_dir}grib_copy -w count=2 ${data_dir}/v.grib2 $temp2
# This should fail as we only blacklisted one key
set +e
${tools_dir}grib_compare -b totalLength $temp1 $temp2 >/dev/null
status=$?
set -e
[ $status -eq 1 ]


rm -f $temp1 $temp2
rm -f $outfile
