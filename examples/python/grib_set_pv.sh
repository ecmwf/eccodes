#!/bin/sh

. ./include.sh

OUTPUT=grib_set_pv.py.temp.grib
$PYTHON $examples_src/grib_set_pv.py
rm $OUTPUT
