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
tempGrib="temp.${label}.grib"     # valid message the checks start from
tempGrib2="temp.${label}.2.grib"  # message modified by a check
tempLog="temp.${label}.log"
tempOut="temp.${label}.out"

samples="regular_gg_ml regular_gg_pl regular_gg_sfc rotated_gg_ml rotated_gg_pl rotated_gg_sfc"

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

# Copy the input into $tempGrib, clearing the bogus "increments given" bits on
# the way if it does not already pass the validity checks. Every check below
# starts from a valid message, so that the only thing wrong with the message it
# ends up looking at is whatever the check changed itself
make_valid_message()
{
   if [ "`${tools_dir}/grib_get -p isMessageValid $1 2>/dev/null`" = 1 ]; then
      cp $1 $tempGrib
   else
      echo "$0: $1 is not valid. Repairing it into $tempGrib"
      case "`${tools_dir}/grib_get -p gridType $1`" in
         healpix) _keys=iDirectionIncrementGiven=0,jDirectionIncrementGiven=0 ;;  # no Di and no Dj
         *)       _keys=jDirectionIncrementGiven=0 ;;                             # Di is given, no Dj
      esac
      ${tools_dir}/grib_set -s $_keys $1 $tempGrib
   fi
   grib_check_key_equals $tempGrib isMessageValid 1
}


echo "Test HEALPix: neither Di nor Dj (GRIB2 only)"
# ---------------------------------------------------------------------------
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
latest=`${tools_dir}/grib_get -p tablesVersionLatest $input`
cat > $tempFilt <<EOF
  set tablesVersion = $latest;
  set gridType = "healpix";
  set longitudeOfFirstGridPointInDegrees = 45;
  set numberOfPointsAlongASide = 1;
  set values = {1,2,3,4,5,6,7,8,9,10,11,12}; # count = 12*N*N
  write;
EOF
${tools_dir}/grib_filter -o $tempGrib2 $tempFilt $input
grib_check_key_equals $tempGrib2 gridType,Nside 'healpix 1'

# Template 3.150 encodes no increments, so none of these keys exists.
# This does not depend on the message being valid
for key in iDirectionIncrement jDirectionIncrement \
           iDirectionIncrementInDegrees jDirectionIncrementInDegrees \
           Di Dj DiInDegrees DjInDegrees; do
  check_key_not_found $tempGrib2 $key
done

# ... hence a valid HEALPix message has both "increments given" bits at zero
make_valid_message $tempGrib2
grib_check_key_equals $tempGrib iDirectionIncrementGiven,jDirectionIncrementGiven,DiGiven,DjGiven '0 0 0 0'

# Setting either bit makes the message invalid (48=i+j, 32=i, 16=j)
for flags in 48 32 16; do
  ${tools_dir}/grib_set -s resolutionAndComponentFlags=$flags $tempGrib $tempGrib2
  check_message_invalid $tempGrib2 "gridType=healpix but [ij]DirectionIncrementGiven=1 (must be 0)"
done


echo "Test Gaussian grids: Di but no Dj"
# ---------------------------------------------------------------------------
# There is no Dj in either edition, but Di is always there
for edition in 1 2; do
  for s in regular_gg_sfc rotated_gg_sfc; do
    input=$ECCODES_SAMPLES_PATH/${s}_grib${edition}.tmpl
    for key in jDirectionIncrement jDirectionIncrementInDegrees Dj DjInDegrees; do
      check_key_not_found $input $key
    done
    grib_check_key_exists $input iDirectionIncrement
  done
done

echo "Test Gaussian grids: GRIB2 has two independent bits"
# ---------------------------------------------------------------------------
for s in $samples; do
  make_valid_message $ECCODES_SAMPLES_PATH/${s}_grib2.tmpl
  grib_check_key_equals $tempGrib iDirectionIncrementGiven,jDirectionIncrementGiven,DiGiven,DjGiven '1 0 1 0'
done

# Turning the j bit on must be reported as invalid
for s in regular_gg rotated_gg; do
  make_valid_message $ECCODES_SAMPLES_PATH/${s}_sfc_grib2.tmpl
  ${tools_dir}/grib_set -s jDirectionIncrementGiven=1 $tempGrib $tempGrib2
  check_message_invalid $tempGrib2 "gridType=$s has no jDirectionIncrement but jDirectionIncrementGiven=1"
done

# Di must agree with iDirectionIncrementGiven
for s in regular_gg rotated_gg; do
  make_valid_message $ECCODES_SAMPLES_PATH/${s}_sfc_grib2.tmpl

  # Given but missing -> invalid
  ${tools_dir}/grib_set -s iDirectionIncrement=missing $tempGrib $tempGrib2
  grib_check_key_equals $tempGrib2 iDirectionIncrementGiven,iDirectionIncrement '1 MISSING'
  check_message_invalid $tempGrib2 "iDirectionIncrementGiven=1 but iDirectionIncrement=missing"

  # Not given but present -> invalid
  ${tools_dir}/grib_set -s iDirectionIncrementGiven=0 $tempGrib $tempGrib2
  check_message_invalid $tempGrib2 "iDirectionIncrementGiven=0 but iDirectionIncrement!=missing"

  # Not given and missing -> valid
  ${tools_dir}/grib_set -s iDirectionIncrementGiven=0,iDirectionIncrement=missing $tempGrib $tempGrib2
  grib_check_key_equals $tempGrib2 iDirectionIncrementGiven,jDirectionIncrementGiven,iDirectionIncrement '0 0 MISSING'
  grib_check_key_equals $tempGrib2 isMessageValid 1
done

echo "Test Gaussian grids: GRIB1 has one bit for both directions"
# ---------------------------------------------------------------------------
for s in $samples; do
  make_valid_message $ECCODES_SAMPLES_PATH/${s}_grib1.tmpl
  # The two keys are aliases of ijDirectionIncrementGiven so they cannot differ.
  # In particular jDirectionIncrementGiven=1 must NOT make the message invalid
  grib_check_key_equals $tempGrib ijDirectionIncrementGiven,iDirectionIncrementGiven,jDirectionIncrementGiven '1 1 1'

  ${tools_dir}/grib_set -s ijDirectionIncrementGiven=0 $tempGrib $tempGrib2
  grib_check_key_equals $tempGrib2 iDirectionIncrementGiven,jDirectionIncrementGiven '0 0'
done

# Di must still agree with the single bit
make_valid_message $ECCODES_SAMPLES_PATH/regular_gg_sfc_grib1.tmpl

${tools_dir}/grib_set -s iDirectionIncrement=missing $tempGrib $tempGrib2
check_message_invalid $tempGrib2 "iDirectionIncrementGiven=1 but iDirectionIncrement=missing"

${tools_dir}/grib_set -s ijDirectionIncrementGiven=0,iDirectionIncrement=missing $tempGrib $tempGrib2
grib_check_key_equals $tempGrib2 isMessageValid 1

echo "Test reduced Gaussian grids are not checked (they have a PL array)"
# ---------------------------------------------------------------------------
for edition in 1 2; do
  input=$ECCODES_SAMPLES_PATH/reduced_gg_sfc_grib${edition}.tmpl
  grib_check_key_equals $input PLPresent,isMessageValid '1 1'
done


# Clean up
rm -f $tempFilt $tempGrib $tempGrib2 $tempLog $tempOut
