#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh
set -u

label="codes_ecc-1392_test_f"
TEMP_ERR=temp.$label.err

# Create files with spaces in various places
# -------------------------------------------
TEMP_INPUT="temp.$label.space in middle"
touch "$TEMP_INPUT"
${examples_dir}/eccodes_f_codes_ecc-1392 "$TEMP_INPUT"
rm -f "$TEMP_INPUT"


TEMP_INPUT=" temp.$label.space in beg"
touch "$TEMP_INPUT"
${examples_dir}/eccodes_f_codes_ecc-1392 "$TEMP_INPUT"
rm -f "$TEMP_INPUT"


# Test non-existent files
# ----------------------------
TEMP_INPUT="temp.nonexistent.file"
set +e
${examples_dir}/eccodes_f_codes_ecc-1392 "$TEMP_INPUT" 2>$TEMP_ERR
status=$?
set -e
[ $status -ne 0 ]
fgrep -q "open_file: (temp.nonexistent.file) Input output problem" $TEMP_ERR


TEMP_INPUT="temp nonexistent file"
set +e
${examples_dir}/eccodes_f_codes_ecc-1392 "$TEMP_INPUT" 2>$TEMP_ERR
status=$?
set -e
[ $status -ne 0 ]
fgrep -q "open_file: (temp nonexistent file) Input output problem" $TEMP_ERR


# Clean up
rm -f $TEMP_ERR
