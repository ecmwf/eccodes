#!/bin/sh

. ./include.sh

label="py_grib_get_keys"
temp1=temp.$label.1.out
temp2=temp.$label.2.out

$PYTHON $examples_src/grib_get_keys.py > $temp1

# Rerun test with no type-check decorator (See GRIB-51)
export ECCODES_PYTHON_NO_TYPE_CHECKS=1
$PYTHON $examples_src/grib_get_keys.py > $temp2

diff $temp1 $temp2

rm -f $temp1 $temp2
