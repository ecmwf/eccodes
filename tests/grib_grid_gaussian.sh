#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

# Direction increments of the (non-reduced) Gaussian grids: regular_gg and rotated_gg
#
# A Gaussian grid has no Dj: the number of parallels between a pole and the
# equator (N) is used instead. Only Di is encoded, and it can be either
# given (present) or not given (missing).
#
# The "increments given" flags are handled differently in the two editions:
#   GRIB2: bits 3 and 4 of the resolution and component flags are separate.
#          Templates 3.40/3.41 have no Dj, so bit 4 must always be zero
#   GRIB1: a single bit (ijDirectionIncrementGiven, octet 17 bit 1) covers both
#          directions, so iDirectionIncrementGiven and jDirectionIncrementGiven
#          are aliases of the very same bit and are always equal

. ./include.ctest.sh

label="grib_grid_gaussian_test"
tempGrib="temp.${label}.grib"
tempLog="temp.${label}.log"
tempOut="temp.${label}.out"

samples="regular_gg_ml regular_gg_pl regular_gg_sfc rotated_gg_ml rotated_gg_pl rotated_gg_sfc"

# There is no jDirectionIncrement in either edition
# ------------------------------------------------
for edition in 1 2; do
  for s in $samples; do
    input=$ECCODES_SAMPLES_PATH/${s}_grib${edition}.tmpl
    for key in jDirectionIncrement jDirectionIncrementInDegrees Dj DjInDegrees; do
      set +e
      ${tools_dir}/grib_get -p $key $input > $tempOut 2> $tempLog
      status=$?
      set -e
      [ $status -ne 0 ]
      grep -q "$key.*not found" $tempLog
    done
    # ... but Di is there
    grib_check_key_exists $input iDirectionIncrement
  done
done

# GRIB2: the two flags are independent, jDirectionIncrementGiven must be 0
# ------------------------------------------------------------------------
for s in $samples; do
  input=$ECCODES_SAMPLES_PATH/${s}_grib2.tmpl
  grib_check_key_equals $input jDirectionIncrementGiven,DjGiven '0 0'
  grib_check_key_equals $input iDirectionIncrementGiven,isMessageValid '1 1'
done

# Turning the j bit on (48 = i and j given) must be reported as invalid
input=$ECCODES_SAMPLES_PATH/regular_gg_sfc_grib2.tmpl
${tools_dir}/grib_set -s resolutionAndComponentFlags=48 $input $tempGrib
${tools_dir}/grib_get -p isMessageValid $tempGrib > $tempOut 2> $tempLog
[ "$(cat $tempOut)" = 0 ]
grep -q "gridType=regular_gg has no jDirectionIncrement but jDirectionIncrementGiven=1" $tempLog

input=$ECCODES_SAMPLES_PATH/rotated_gg_sfc_grib2.tmpl
${tools_dir}/grib_set -s resolutionAndComponentFlags=48 $input $tempGrib
${tools_dir}/grib_get -p isMessageValid $tempGrib > $tempOut 2> $tempLog
[ "$(cat $tempOut)" = 0 ]
grep -q "gridType=rotated_gg has no jDirectionIncrement but jDirectionIncrementGiven=1" $tempLog

# GRIB2: Di must agree with iDirectionIncrementGiven
# ---------------------------------------------------
for s in regular_gg_sfc rotated_gg_sfc; do
  input=$ECCODES_SAMPLES_PATH/${s}_grib2.tmpl

  # iDirectionIncrementGiven=1 but Di missing -> invalid
  ${tools_dir}/grib_set -s iDirectionIncrement=missing $input $tempGrib
  grib_check_key_equals $tempGrib iDirectionIncrementGiven,iDirectionIncrement '1 MISSING'
  ${tools_dir}/grib_get -p isMessageValid $tempGrib > $tempOut 2> $tempLog
  [ "$(cat $tempOut)" = 0 ]
  grep -q "iDirectionIncrementGiven=1 but iDirectionIncrement=missing" $tempLog

  # iDirectionIncrementGiven=0 but Di present -> invalid
  ${tools_dir}/grib_set -s iDirectionIncrementGiven=0 $input $tempGrib
  ${tools_dir}/grib_get -p isMessageValid $tempGrib > $tempOut 2> $tempLog
  [ "$(cat $tempOut)" = 0 ]
  grep -q "iDirectionIncrementGiven=0 but iDirectionIncrement!=missing" $tempLog

  # iDirectionIncrementGiven=0 and Di missing -> valid
  ${tools_dir}/grib_set -s iDirectionIncrementGiven=0,iDirectionIncrement=missing $input $tempGrib
  grib_check_key_equals $tempGrib iDirectionIncrementGiven,jDirectionIncrementGiven,iDirectionIncrement '0 0 MISSING'
  grib_check_key_equals $tempGrib isMessageValid 1
done

# GRIB1: one single bit for both directions
# ------------------------------------------
for s in $samples; do
  input=$ECCODES_SAMPLES_PATH/${s}_grib1.tmpl
  # Both keys are aliases of ijDirectionIncrementGiven so they cannot differ
  grib_check_key_equals $input ijDirectionIncrementGiven,iDirectionIncrementGiven,jDirectionIncrementGiven '1 1 1'
  # jDirectionIncrementGiven=1 must NOT make the message invalid in GRIB1
  grib_check_key_equals $input isMessageValid 1

  ${tools_dir}/grib_set -s ijDirectionIncrementGiven=0 $input $tempGrib
  grib_check_key_equals $tempGrib iDirectionIncrementGiven,jDirectionIncrementGiven '0 0'
done

# GRIB1: Di must still agree with the single bit
input=$ECCODES_SAMPLES_PATH/regular_gg_sfc_grib1.tmpl
${tools_dir}/grib_set -s iDirectionIncrement=missing $input $tempGrib
${tools_dir}/grib_get -p isMessageValid $tempGrib > $tempOut 2> $tempLog
[ "$(cat $tempOut)" = 0 ]
grep -q "iDirectionIncrementGiven=1 but iDirectionIncrement=missing" $tempLog

${tools_dir}/grib_set -s ijDirectionIncrementGiven=0,iDirectionIncrement=missing $input $tempGrib
grib_check_key_equals $tempGrib isMessageValid 1

# Reduced Gaussian grids are not checked (they have a PL array)
# -------------------------------------------------------------
for edition in 1 2; do
  input=$ECCODES_SAMPLES_PATH/reduced_gg_sfc_grib${edition}.tmpl
  grib_check_key_equals $input PLPresent,isMessageValid '1 1'
done

rm -f $tempGrib $tempLog $tempOut
