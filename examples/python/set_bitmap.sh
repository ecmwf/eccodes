#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON set_bitmap.py 2> $REDIRECT > $REDIRECT
rm out.grib || true
