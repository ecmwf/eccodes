#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON $examples_src/samples.py
rm -f out.samples.grib
