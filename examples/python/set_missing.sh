#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON set_missing.py 2> $REDIRECT > $REDIRECT
rm out.grib || true
