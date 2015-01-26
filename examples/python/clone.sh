#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON clone.py 2> $REDIRECT > $REDIRECT
rm -f out.clone.grib || true
