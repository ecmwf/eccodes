#!/bin/ksh
set -e
job=$1
out=$2
$job > $out 2>&1 &
