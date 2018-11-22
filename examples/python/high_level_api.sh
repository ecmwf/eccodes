#!/bin/sh

. ./include.sh

# To get verbose output
#$PYTHON -m unittest -v high_level_api

$PYTHON $examples_src/high_level_api.py -v

rm -f test.index
