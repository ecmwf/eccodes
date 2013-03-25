#!/bin/sh

. ./include.sh

REDIRECT=/dev/null
$PYTHON nearest.py 2> $REDIRECT > $REDIRECT
