#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON $examples_src/samples.py 2> $REDIRECT > $REDIRECT
rm -f out.samples.grib || true
