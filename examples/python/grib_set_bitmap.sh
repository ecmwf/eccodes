#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
OUT_TMP=out.set_bitmap_p.grib

$PYTHON $examples_src/grib_set_bitmap.py

x=`${tools_dir}/grib_get -p numberOfDataPoints,numberOfCodedValues,numberOfMissing $OUT_TMP`
[ "$x" = "496 446 50" ]

rm -f $OUT_TMP
