#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON $examples_src/grib_set_missing.py 2> $REDIRECT > $REDIRECT
rm -f out.p_set_missing.grib
