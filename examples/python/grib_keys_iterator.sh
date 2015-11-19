#!/bin/sh

. ./include.sh

TEMP1=temp1.$$
TEMP2=temp2.$$

$PYTHON $examples_src/grib_keys_iterator.py 2> $TEMP1 > $TEMP1
./p_grib_keys_iterator ../../data/reduced_latlon_surface.grib1 2> $TEMP2 > $TEMP2

diff $TEMP1 $TEMP2
rm $TEMP1 $TEMP2 || true
