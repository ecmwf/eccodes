#!/bin/sh

. ./include.sh

# To get verbose output
#$PYTHON -m unittest -v high_level_api

if [ $HAVE_PRODUCT_GRIB -eq 1 ]; then
    echo 'Running tests for GRIB ...'
    $PYTHON $examples_src/high_level_api_grib.py -v
    rm -f test.index
fi

if [ $HAVE_PRODUCT_BUFR -eq 1 ]; then
    echo 'Running tests for BUFR ...'
    $PYTHON $examples_src/high_level_api_bufr.py -v
    rm -f test.index
fi
