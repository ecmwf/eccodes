#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON set_missing.py 2> $REDIRECT > $REDIRECT
rm -f out.set_missing.grib || true
