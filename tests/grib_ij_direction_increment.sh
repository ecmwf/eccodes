#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

# ECC-2336: i/j direction increments and the "increments given" flags.
# See MessageIsValid::check_grid_increments
#
#   gridType     templates     Di   Dj                    flags
#   healpix      3.150         no   no                    i=0 and j=0
#   regular_gg   3.40  / 3.0   yes  no (N is used)        j=0, i=1 if Di is set
#   rotated_gg   3.41  / 3.10  yes  no (N is used)        j=0, i=1 if Di is set
#
# The flags are encoded differently in the two editions:
#   GRIB2: bits 3 and 4 of resolutionAndComponentFlags are independent
#   GRIB1: a single bit (ijDirectionIncrementGiven, octet 17 bit 1) covers both
#          directions, so iDirectionIncrementGiven and jDirectionIncrementGiven
#          are aliases of the very same bit and can never differ

. ./include.ctest.sh

label="grib_ij_direction_increment_test"
tempFilt="temp.${label}.filt"
tempGrib="temp.${label}.grib"
tempGrib2="temp.${label}.2.grib"
tempLog="temp.${label}.log"
tempOut="temp.${label}.out"

# Check the given key is not defined for the given message
check_key_not_found()
{
   set +e
   ${tools_dir}/grib_get -p $2 $1 > $tempOut 2> $tempLog
   _status=$?
   set -e
   [ $_status -ne 0 ]
   grep -q "$2.*not found" $tempLog
}

# Check isMessageValid is 0 and that the reason matches the given pattern
check_message_invalid()
{
   ${tools_dir}/grib_get -p isMessageValid $1 > $tempOut 2> $tempLog
   [ "`cat $tempOut`" = 0 ]
   grep -q "$2" $tempLog
}


echo "Test HEALPix: neither Di nor Dj (GRIB2 only)"
# ---------------------------------------------------------------------------
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
latest=`${tools_dir}/grib_get -p tablesVersionLatest $input`
cat > $tempFilt <<EOF
  set tablesVersion = $latest;
  set gridType = "healpix";
  set longitudeOfFirstGridPointInDegrees = 45;
  set resolutionAndComponentFlags = 0; # GRIB2.tmpl has both "increments given" bits set
  set numberOfPointsAlongASide = 1;
  set values = {1,2,3,4,5,6,7,8,9,10,11,12}; # count = 12*N*N
  write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $input
grib_check_key_equals $tempGrib gridType,Nside 'healpix 1'

# Template 3.150 encodes no increments, so none of these keys exists
for key in iDirectionIncrement jDirectionIncrement \
           iDirectionIncrementInDegrees jDirectionIncrementInDegrees \
           Di Dj DiInDegrees DjInDegrees; do
  check_key_not_found $tempGrib $key
done

# ... hence both "increments given" bits must be zero (32=i, 16=j)
grib_check_key_equals $tempGrib isMessageValid 1
for flags in 32 16; do
  ${tools_dir}/grib_set -s resolutionAndComponentFlags=$flags $tempGrib $tempGrib2
  check_message_invalid $tempGrib2 "gridType=healpix but [ij]DirectionIncrementGiven=1 (must be 0)"
done


echo "Test Gaussian grids: Di but no Dj"
# ---------------------------------------------------------------------------
# There is no Dj in either edition, but Di is always there
for edition in 1 2; do
  for s in regular_gg rotated_gg; do
    input=$ECCODES_SAMPLES_PATH/${s}_sfc_grib${edition}.tmpl
    for key in jDirectionIncrement jDirectionIncrementInDegrees Dj DjInDegrees; do
      check_key_not_found $input $key
    done
    grib_check_key_exists $input iDirectionIncrement
  done
done

echo "Test Gaussian grids: GRIB2 has two independent bits"
# ---------------------------------------------------------------------------
for s in regular_gg rotated_gg; do
  input=$ECCODES_SAMPLES_PATH/${s}_sfc_grib2.tmpl

  # Turning the j bit on must be reported as invalid
  ${tools_dir}/grib_set -s jDirectionIncrementGiven=1 $input $tempGrib2
  check_message_invalid $tempGrib2 "gridType=$s has no jDirectionIncrement but jDirectionIncrementGiven=1"

  # Di must agree with iDirectionIncrementGiven:
  # Given but missing -> invalid
  ${tools_dir}/grib_set -s iDirectionIncrement=missing $input $tempGrib2
  check_message_invalid $tempGrib2 "iDirectionIncrementGiven=1 but iDirectionIncrement=missing"

  # Not given but present -> invalid
  ${tools_dir}/grib_set -s iDirectionIncrementGiven=0 $input $tempGrib2
  check_message_invalid $tempGrib2 "iDirectionIncrementGiven=0 but iDirectionIncrement!=missing"

  # Not given and missing -> valid
  ${tools_dir}/grib_set -s iDirectionIncrementGiven=0,iDirectionIncrement=missing $input $tempGrib2
  grib_check_key_equals $tempGrib2 isMessageValid 1
done

echo "Test Gaussian grids: GRIB1 has one bit for both directions"
# ---------------------------------------------------------------------------
for s in regular_gg rotated_gg; do
  input=$ECCODES_SAMPLES_PATH/${s}_sfc_grib1.tmpl
  # The two keys are aliases of ijDirectionIncrementGiven so they cannot differ.
  # In particular jDirectionIncrementGiven=1 must NOT make the message invalid
  grib_check_key_equals $input \
     ijDirectionIncrementGiven,iDirectionIncrementGiven,jDirectionIncrementGiven,isMessageValid '1 1 1 1'

  ${tools_dir}/grib_set -s ijDirectionIncrementGiven=0 $input $tempGrib2
  grib_check_key_equals $tempGrib2 iDirectionIncrementGiven,jDirectionIncrementGiven '0 0'
done


# Clean up
rm -f $tempFilt $tempGrib $tempGrib2 $tempLog $tempOut
