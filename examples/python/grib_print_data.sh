#!/bin/sh

. ./include.sh

TEMP1=temp1.$$
TEMP2=temp2.$$

$PYTHON $examples_src/grib_print_data.py 2> $TEMP1 > $TEMP1
./p_grib_print_data ../../data/regular_latlon_surface.grib1 2> $TEMP2 > $TEMP2

diff $TEMP1 $TEMP2
rm $TEMP1 $TEMP2 || true
