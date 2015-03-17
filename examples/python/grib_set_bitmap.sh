#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON grib_set_bitmap.py 2> $REDIRECT > $REDIRECT
rm out.bmp.grib || true
