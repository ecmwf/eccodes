#!/bin/bash
# SPDX-FileCopyrightText: 2005- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Downloads the test data that data/**/*_files.txt list into <dest>/data, laid out as
# the build tree's data/, many files at a time; ctest's download tests fetch one by one.
set -euo pipefail
dest="$1"
base="${ECBUILD_DOWNLOAD_BASE_URL:-https://sites.ecmwf.int/repository}/eccodes/test-data"
cd "$(dirname "$0")/.."
{
  for list in data/*_files.txt data/*/*_files.txt; do
    sed 's/[[:space:]]*$//; /^$/d' "$list" | sed "s|^|$(dirname "$list")/|"
  done
  # Appended in data/CMakeLists.txt and data/bufr/CMakeLists.txt.
  printf '%s\n' data/ccsds.grib2 data/ccsds_szip.grib2 data/bufr/vos308014_v3_26.bufr data/bufr/bad.bufr
} | sort -u | xargs -P 16 -I{} curl --silent --show-error --fail --retry 3 --create-dirs -o "$dest/{}" "$base/{}"
