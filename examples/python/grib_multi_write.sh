#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON $examples_src/grib_multi_write.py 2> $REDIRECT > $REDIRECT
rm -f out.mw.grib || true
