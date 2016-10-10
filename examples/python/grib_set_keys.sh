#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON $examples_src/grib_set_keys.py # 2> $REDIRECT > $REDIRECT
rm -f out.set.grib || true
