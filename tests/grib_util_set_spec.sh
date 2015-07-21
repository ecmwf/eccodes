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

infile=../data/latlon.grib
outfile=out.grib_util_set_spec.grib

${test_dir}/grib_util_set_spec $infile $outfile > /dev/null

res=`${tools_dir}grib_get -p Ni,Nj,numberOfValues,bitsPerValue $outfile`
[ "$res" = "17 14 238 24" ]

${tools_dir}grib_get_data $outfile > /dev/null

rm -f $outfile
