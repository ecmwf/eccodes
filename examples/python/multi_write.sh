#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON multi_write.py 2> $REDIRECT > $REDIRECT
rm -f out.mw.grib || true
