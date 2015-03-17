#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON grib_clone.py 2> $REDIRECT > $REDIRECT
rm -f out.clone.grib || true
