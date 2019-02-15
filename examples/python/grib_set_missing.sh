#!/bin/sh

. ./include.sh

temp=out.p_set_missing.grib

REDIRECT=/dev/null
$PYTHON $examples_src/grib_set_missing.py 2> $REDIRECT > $REDIRECT
x=`${tools_dir}/grib_get -p scaleFactorOfFirstFixedSurface,scaledValueOfFirstFixedSurface $temp`
[ "$x" = "MISSING MISSING" ]

rm -f $temp
