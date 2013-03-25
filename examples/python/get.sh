#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON get.py 2> $REDIRECT > $REDIRECT

# Rerun test with no type check decorator (See GRIB-51)
export GRIB_API_PYTHON_NO_TYPE_CHECKS=1
$PYTHON get.py 2> $REDIRECT > $REDIRECT
