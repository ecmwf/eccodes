#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON set.py 2> $REDIRECT > $REDIRECT
rm out.grib || true
