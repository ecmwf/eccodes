#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON clone.py 2> $REDIRECT > $REDIRECT
rm out.grib || true
