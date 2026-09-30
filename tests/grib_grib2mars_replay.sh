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

# ---------------------------------------------------------------------------
# Replay the recorded "grib2mars" encoder requests and check the MARS keys
#
# data/grib2mars_replay.zst is a Zstandard compressed JSON-lines file. Every
# line describes one message: the MARS keys it must be labelled with ("mars")
# and the sequence of set/set_missing/clone operations the encoder applied to
# produce it ("out.operations").
#
# The test pipes the decompressed stream into tests/grib_grib2mars_replay,
# which for every line clones the GRIB2.tmpl sample, replays the operations
# onto it and checks that a flat "centre marsClass ... packingType" string
# matches the "origin class ... packing" string built from the "mars" object
# (same approach as mirco's grib_check.cpp). Any mismatch is a hard failure:
# the engine writes a self-contained reproduction block (line number,
# expected vs. got, the failing JSON, and the shell commands to replay it) to
# a log file and exits non-zero; the shell wrapper then prints that log so it
# ends up in the ctest output.
# ---------------------------------------------------------------------------

label="grib_grib2mars_replay_test"

tempOut=temp.$label.out
tempLog=temp.$label.log

input=${data_dir}/grib2mars_replay.zst
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

if ! command -v zstd > /dev/null 2>&1; then
  echo "$label: zstd not found. Test disabled"
  exit 0
fi

# The reference stream was recorded on top of eccodes' eckit-geo path, so the
# test only makes sense when ecCodes was built with eckit-geo support AND the
# ECCODES_ECKIT_GEO runtime switch is on
if [ "${HAVE_ECKIT_GEO:-0}" -ne 1 ]; then
  echo "$label: ecCodes built without eckit-geo (HAVE_ECKIT_GEO=0). Test disabled"
  exit 0
fi

export ECCODES_ECKIT_GEO=1

rm -f $tempOut $tempLog
set +e
zstd -dc --long=31 "$input" | \
    ${test_dir}/grib_grib2mars_replay \
        --log "$tempLog" \
        --input "$input" \
        "$sample_grib2" > $tempOut 2>&1
rc=$?
set -e

cat $tempOut

if [ "$rc" -ne 0 ]; then
  echo ""
  echo "$label: mismatches found. Detailed report:"
  echo "  $tempLog"
  echo "----------------------------------------------------------------------"
  cat $tempLog
  echo "----------------------------------------------------------------------"
  exit 1
fi

# Clean up
rm -f $tempOut $tempLog
