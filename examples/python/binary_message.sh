#!/bin/sh

. ./include.sh

INPUT=${data_dir}/tigge_pf_ecmwf.grib2
OUTPUT=out.grib

$PYTHON $examples_src/binary_message.py $INPUT $OUTPUT
${tools_dir}/grib_compare $INPUT $OUTPUT
rm $OUTPUT || true

# GRIB-568 corrupt grib
# We are expecting the command to fail so must turn off "set -e"
INPUT=${data_dir}/bad.grib
set +e
$PYTHON $examples_src/binary_message.py $INPUT $OUTPUT 2>/dev/null
status=$?
set -e
# Check command did indeed fail
[ $status != 0 ]
rm $OUTPUT || true

