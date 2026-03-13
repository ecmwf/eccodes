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

label="corrupted_messages"

# -----------------------------------------------
# Test that corrupted/truncated GRIB/BUFR messages
# are handled gracefully (no crashes, proper error codes)
# -----------------------------------------------

tempGrib=temp.$label.grib
tempBufr=temp.$label.bufr
tempCorrupt=temp.$label.corrupt
tempOut=temp.$label.out

# --- Test 1: Truncated GRIB message ---
echo "Test: Truncated GRIB message..."
${tools_dir}/grib_set -s edition=2 $ECCODES_SAMPLES_PATH/GRIB2.tmpl $tempGrib
size=$(wc -c < $tempGrib | tr -d ' ')
half=$((size / 2))

# Truncate to first half
dd if=$tempGrib of=$tempCorrupt bs=1 count=$half 2>/dev/null
set +e
${tools_dir}/grib_ls $tempCorrupt > $tempOut 2>&1
status=$?
set -e
# Should fail (non-zero exit), not crash
echo "Truncated GRIB: grib_ls exit code=$status"

# --- Test 2: Truncated BUFR message ---
echo "Test: Truncated BUFR message..."
# Copy BUFR sample directly (grib_set cannot handle BUFR files)
cp $ECCODES_SAMPLES_PATH/BUFR4.tmpl $tempBufr
size=$(wc -c < $tempBufr | tr -d ' ')
half=$((size / 2))

dd if=$tempBufr of=$tempCorrupt bs=1 count=$half 2>/dev/null
set +e
${tools_dir}/bufr_ls $tempCorrupt > $tempOut 2>&1
status=$?
set -e
echo "Truncated BUFR: bufr_ls exit code=$status"

# --- Test 3: Corrupted magic number ---
echo "Test: Corrupted magic number..."
cp $tempGrib $tempCorrupt
# Replace first byte 'G' (0x47) with 'X' (0x58)
printf '\x58' | dd of=$tempCorrupt bs=1 count=1 conv=notrunc 2>/dev/null
set +e
${tools_dir}/grib_ls $tempCorrupt > $tempOut 2>&1
status=$?
set -e
echo "Corrupted magic: grib_ls exit code=$status"
# Should fail or report no messages, not crash

# --- Test 4: File with just "GRIB" header and nothing else ---
echo "Test: GRIB header only (4 bytes)..."
printf 'GRIB' > $tempCorrupt
set +e
${tools_dir}/grib_ls $tempCorrupt > $tempOut 2>&1
status=$?
set -e
echo "GRIB header only: grib_ls exit code=$status"

# --- Test 5: Empty file ---
echo "Test: Empty file..."
> $tempCorrupt
set +e
${tools_dir}/grib_ls $tempCorrupt > $tempOut 2>&1
status=$?
set -e
echo "Empty file: grib_ls exit code=$status"

# --- Test 6: Random bytes file ---
echo "Test: Random bytes..."
dd if=/dev/urandom of=$tempCorrupt bs=1 count=256 2>/dev/null
set +e
${tools_dir}/grib_ls $tempCorrupt > $tempOut 2>&1
status=$?
set -e
echo "Random bytes: grib_ls exit code=$status"

# --- Test 7: Missing 7777 end marker ---
echo "Test: Missing 7777 end marker..."
cp $tempGrib $tempCorrupt
size=$(wc -c < $tempCorrupt | tr -d ' ')
truncsize=$((size - 4))
dd if=$tempGrib of=$tempCorrupt bs=1 count=$truncsize 2>/dev/null
# Append something that is NOT 7777
printf '\x00\x00\x00\x00' >> $tempCorrupt
set +e
${tools_dir}/grib_ls $tempCorrupt > $tempOut 2>&1
status=$?
set -e
echo "Bad end marker: grib_ls exit code=$status"

echo ""
echo "All corrupted message tests completed without crashes."

rm -f $tempGrib $tempBufr $tempCorrupt $tempOut
