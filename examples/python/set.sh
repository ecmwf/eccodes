#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON set.py 2> $REDIRECT > $REDIRECT
rm -f out.set.grib || true
