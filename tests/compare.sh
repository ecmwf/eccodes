#!/bin/sh
# Copyright 2005-2014 ECMWF.
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
outfile=${infile}.1

rm -f $outfile || true

${tools_dir}grib_set -s shortName=2d $infile $outfile

${tools_dir}grib_compare -b indicatorOfParameter,paramId,shortName $infile $outfile > $REDIRECT

# Test the -r switch
infile=${data_dir}/v.grib2
for i in 1 2 3; do
  ${tools_dir}grib_copy -wcount=$i $infile temp_comp.$i
done
cat temp_comp.1 temp_comp.2 temp_comp.3 > temp_comp.123
cat temp_comp.3 temp_comp.2 temp_comp.1 > temp_comp.321

# Compare files in which the messages are not in the same order
${tools_dir}grib_compare -r temp_comp.123 temp_comp.321

rm -f temp_comp.1 temp_comp.2 temp_comp.3 temp_comp.123 temp_comp.321
rm -f $outfile || true
