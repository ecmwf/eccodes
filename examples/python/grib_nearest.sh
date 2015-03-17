#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON grib_nearest.py 2> $REDIRECT > $REDIRECT
