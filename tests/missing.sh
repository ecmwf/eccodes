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

infile="${data_dir}/missing.grib2"
outfile="${data_dir}/missing_new.grib2"

scaleFactorOfSecondFixedSurface=`${tools_dir}grib_get -w count=1 -p scaleFactorOfSecondFixedSurface $infile`
[ "$scaleFactorOfSecondFixedSurface" = "0" ]

scaledValueOfSecondFixedSurface=`${tools_dir}grib_get -w count=1 -p scaledValueOfSecondFixedSurface $infile`
[ "$scaledValueOfSecondFixedSurface" = "0" ]

${tools_dir}grib_set -s scaleFactorOfSecondFixedSurface=missing,scaledValueOfSecondFixedSurface=missing $infile $outfile

scaleFactorOfSecondFixedSurface=`${tools_dir}grib_get -w count=1 -p scaleFactorOfSecondFixedSurface $outfile`
[ "$scaleFactorOfSecondFixedSurface" = "MISSING" ]

scaledValueOfSecondFixedSurface=`${tools_dir}grib_get -w count=1 -p scaledValueOfSecondFixedSurface $outfile`
[ "$scaledValueOfSecondFixedSurface" = "MISSING" ]

set +e

${tools_dir}grib_set -s centre=missing $infile $outfile 2> $REDIRECT > $REDIRECT

[ $? -ne 0 ]

rm -f $outfile
