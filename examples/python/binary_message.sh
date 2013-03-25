#!/bin/sh

. ./include.sh

REDIRECT=/dev/null

INPUT=${data_dir}/tigge_pf_ecmwf.grib2
OUTPUT=out.grib

$PYTHON binary_message.py $INPUT $OUTPUT 2> $REDIRECT > $REDIRECT
${tools_dir}/grib_compare $INPUT $OUTPUT
rm $OUTPUT || true
