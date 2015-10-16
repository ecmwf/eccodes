#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON $examples_src/grib_nearest.py 2> $REDIRECT > $REDIRECT
