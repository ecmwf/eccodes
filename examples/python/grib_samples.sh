#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON $examples_src/grib_samples.py
rm -f p_out.grib_samples.grib
