#!/bin/sh

. ./include.sh

$PYTHON $examples_src/grib_index.py
rm -f my.idx
