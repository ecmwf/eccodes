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

if [ "$ECCODES_ON_WINDOWS" -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi

label="grib_parameter_concept_lookup_test"
tempDir=$label.temp.dir
rm -rf "$tempDir"
mkdir -p "$tempDir/definitions/grib2/localConcepts/ecmf"
trap 'rm -rf "$tempDir"' EXIT

# Use an ordinary GRIB2 message and an isolated definitions overlay.
$tools_dir/grib_set -s centre=ecmf,tablesVersion=35,discipline=0,parameterCategory=0,parameterNumber=0,typeOfFirstFixedSurface=1 \
    "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" "$tempDir/input.grib"

write_defs()
{
    prefix=$1
    cat <<EOF
'${prefix}001' = { discipline=1; parameterCategory=0; parameterNumber=0; }
'${prefix}002' = { discipline=0; parameterCategory=0; parameterNumber=0; }
'${prefix}003' = { parameterNumber=0; discipline=0; parameterCategory=0; }
'${prefix}004' = { discipline=0; parameterCategory=0; parameterNumber=0; }
'${prefix}005' = { discipline=0; parameterCategory=0; parameterNumber=1; }
'${prefix}006' = { parameterNumber=1; discipline=0; parameterCategory=0; }
'${prefix}007' = { parameterNumber=2; discipline=0; parameterCategory=0; }
'${prefix}008' = { parameterNumber=3; discipline=0; parameterCategory=0; typeOfFirstFixedSurface=1; }
'${prefix}009' = { discipline=0; parameterCategory=0; parameterNumber=3; }
EOF
}

for dir in "$tempDir/definitions/grib2" "$tempDir/definitions/grib2/localConcepts/ecmf"; do
    write_defs 987 > "$dir/paramId.def"
    write_defs ecc_test_ > "$dir/shortName.def"
    write_defs 986 > "$dir/paramId.lte33.def"
    write_defs ecc_legacy_ > "$dir/shortName.lte33.def"
done

export ECCODES_DEFINITION_PATH="$PWD/$tempDir/definitions:$ECCODES_DEFINITION_PATH"

# Equal scores: the later rule wins, irrespective of which path it uses.
# Repeat on the same handle to ensure only rules, never results, are cached.
cat > "$tempDir/test.filter" <<'EOF'
assert(paramId == 987004);
assert(shortName is "ecc_test_004");
set parameterNumber = 1;
assert(paramId == 987006);
assert(shortName is "ecc_test_006");

# No indexed match, but a reordered rule still matches.
set parameterNumber = 2;
assert(paramId == 987007);
assert(shortName is "ecc_test_007");

# A more specific fallback wins over a later, less specific indexed rule.
set parameterNumber = 3;
assert(paramId == 987008);
assert(shortName is "ecc_test_008");

# Encoding by concept name still uses the existing reverse lookup.
set paramId = 987004;
assert(parameterNumber == 0);
set shortName = "ecc_test_005";
assert(parameterNumber == 1);
assert(paramId == 987006);

# Changing table version selects a different definitions file.
set tablesVersion = 33;
assert(paramId == 986006);
assert(shortName is "ecc_legacy_006");
set tablesVersion = 35;
assert(paramId == 987006);
assert(shortName is "ecc_test_006");
EOF
$tools_dir/grib_filter "$tempDir/test.filter" "$tempDir/input.grib"
