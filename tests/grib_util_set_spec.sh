#!/bin/sh
# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

### Regular Lat/Lon Grid
###########################################
infile=../data/latlon.grib
outfile=out.grib_util_set_spec.grib
rm -f $outfile

${test_dir}/grib_util_set_spec $infile $outfile > /dev/null

res=`${tools_dir}grib_get -p Ni,Nj,numberOfValues,bitsPerValue $outfile`
[ "$res" = "17 14 238 24" ]

${tools_dir}grib_get_data $outfile > /dev/null

### Reduced Gaussian Grid
###########################################
infile=$ECCODES_SAMPLES_PATH/reduced_gg_pl_320_grib2.tmpl
outfile=out.grib_util_set_spec.grib
rm -f $outfile

${test_dir}/grib_util_set_spec $infile $outfile

${tools_dir}grib_get_data $outfile > /dev/null


### Clean up
rm -f $outfile
