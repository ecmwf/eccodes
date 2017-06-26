#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON $examples_src/grib_get_keys.py 2> $REDIRECT > $REDIRECT

# Rerun test with no type-check decorator (See GRIB-51)
export ECCODES_PYTHON_NO_TYPE_CHECKS=1
$PYTHON $examples_src/grib_get_keys.py 2> $REDIRECT > $REDIRECT
