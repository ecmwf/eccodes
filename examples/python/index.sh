#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON index.py 2> $REDIRECT > $REDIRECT
rm my.idx || true
