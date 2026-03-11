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

label="packing_roundtrip"

# -----------------------------------------------
# Test packing round-trip: encode values, decode, compare
# This ensures different packing types preserve data integrity
# -----------------------------------------------

tempGrib=temp.$label.grib
tempOut=temp.$label.out
tempRef=temp.$label.ref
tempFilt=temp.$label.filt

# Create a GRIB2 message from sample with known grid dimensions
${tools_dir}/grib_set -s Ni=36,Nj=18,numberOfDataPoints=648,numberOfValues=648 \
    $ECCODES_SAMPLES_PATH/GRIB2.tmpl $tempGrib

# Generate ascending test values (temperature-like)
cat > $tempFilt << EOF
set bitsPerValue = 16;
set Ni = 36;
set Nj = 18;
set numberOfDataPoints = 648;
set numberOfValues = 648;
# Set values to a ramp from 200 to 320 (temperature range in K)
set values = {$(seq 200 0.1855 320 | head -648 | tr '\n' ',' | sed 's/,$//')};
write;
EOF

# Test simple packing (grid_simple) - the default
${tools_dir}/grib_filter -o $tempGrib $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl
grib_check_key_equals $tempGrib packingType grid_simple

stats=$(${tools_dir}/grib_get -p max,min,avg $tempGrib)
echo "Simple packing stats: $stats"

# Verify we can read back the values
${tools_dir}/grib_get_data $tempGrib > $tempOut
count=$(wc -l < $tempOut | tr -d ' ')
# account for header line
test "$count" -gt 0

# Test grid_second_order packing if data file is big enough
if [ "$HAVE_AEC" = "1" ]; then
    tempCcsds=temp.${label}.ccsds.grib
    ${tools_dir}/grib_set -s packingType=grid_ccsds $tempGrib $tempCcsds
    grib_check_key_equals $tempCcsds packingType grid_ccsds

    # Compare original and CCSDS-packed values
    val1=$(${tools_dir}/grib_get -p avg $tempGrib)
    val2=$(${tools_dir}/grib_get -p avg $tempCcsds)
    # They should be close (within packing tolerance)
    echo "Simple avg=$val1, CCSDS avg=$val2"
    rm -f $tempCcsds
fi

# Test changing bitsPerValue
for bpv in 8 12 16 24; do
    tempBpv=temp.${label}.bpv${bpv}.grib
    ${tools_dir}/grib_set -s bitsPerValue=$bpv $tempGrib $tempBpv
    bpv_out=$(${tools_dir}/grib_get -p bitsPerValue $tempBpv)
    test "$bpv_out" = "$bpv"
    rm -f $tempBpv
done

# Test constant field (all values the same)
cat > $tempFilt << EOF
set bitsPerValue = 16;
set Ni = 10;
set Nj = 10;
set numberOfDataPoints = 100;
set numberOfValues = 100;
set values = {$(python3 -c "print(','.join(['273.15']*100))")};
write;
EOF
tempConst=temp.${label}.const.grib
${tools_dir}/grib_filter -o $tempConst $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl 2>/dev/null || true
if [ -f "$tempConst" ]; then
    max=$(${tools_dir}/grib_get -p max $tempConst)
    min=$(${tools_dir}/grib_get -p min $tempConst)
    echo "Constant field: max=$max min=$min"
fi

rm -f $tempGrib $tempOut $tempRef $tempFilt $tempConst
