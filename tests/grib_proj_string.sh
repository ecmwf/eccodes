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

label="grib_proj_string_test"
tempGrib=temp.$label.grib
tempText=temp.$label.txt
grib2_sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

files="
  mercator.grib2
  satellite.grib
"

# Decide if we have the proj commandline tool
PROJ_NAME="proj"
PROJ_TOOL=""
if command -v $PROJ_NAME >/dev/null 2>&1; then
    PROJ_TOOL=$PROJ_NAME
fi

for f in `echo $files`; do
    file=${data_dir}/$f
    # (lon, lat): as WGS84 (EPSG:4326), or on the message figure (eckit::geo)
    ps=`${tools_dir}/grib_get -wcount=1 -p projSourceString $file`
    case $ps in
      EPSG:4326) [ "${ECCODES_ECKIT_GEO:-0}" -eq 0 ];;
      "+proj=longlat "*) [ "${ECCODES_ECKIT_GEO:-0}" -ne 0 ];;
      *) echo "ERROR: File: $file. Invalid proj source string: |$ps|"; exit 1;;
    esac
    ps=`${tools_dir}/grib_get -wcount=1 -p projTargetString $file`
    # Check length of result is nonzero
    [ -n "$ps" ]
    # Check contents (basic)
    case $ps in
      *+proj=*) echo OK;;
      *)        echo "ERROR: File: $file. Invalid proj string: |$ps|"; exit 1;;
    esac
    if test "x$PROJ_TOOL" != "x"; then
        ${tools_dir}/grib_get -p longitudeOfFirstGridPointInDegrees,latitudeOfFirstGridPointInDegrees $file |\
            $PROJ_TOOL $ps
    fi
done

# Various grid types
# ECC-1552: To be done later. Current behaviour is to return KeyNotFound
# set +e
# ${tools_dir}/grib_get -p projString $grib2_sample > $tempText 2>&1
# status=$?
# set -e
# [ $status -ne 0 ]
# grep -q "Key/value not found" $tempText
rm -f $tempText

# Unprojected grids: as WGS84, or on the message figure (eckit::geo)
longlat="+proj=longlat +datum=WGS84"
[ "${ECCODES_ECKIT_GEO:-0}" -eq 0 ] || longlat="+proj=longlat +R="
${tools_dir}/grib_get -p projString $grib2_sample > $tempText
grep -q "$longlat" $tempText
${tools_dir}/grib_get -p projString $ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl > $tempText
grep -q "$longlat" $tempText

# ${tools_dir}/grib_get -p projString $ECCODES_SAMPLES_PATH/regular_ll_pl_grib2.tmpl > $tempText
# grep -q "proj=longlat +R=6371229" $tempText
# ${tools_dir}/grib_get -p projString $ECCODES_SAMPLES_PATH/regular_gg_ml_grib1.tmpl > $tempText
# grep -q "proj=longlat +R=6367470" $tempText
# ${tools_dir}/grib_get -p projString $ECCODES_SAMPLES_PATH/reduced_ll_sfc_grib1.tmpl > $tempText
# grep -q "proj=longlat +R=6367470" $tempText


${tools_dir}/grib_set -s gridType=lambert_azimuthal_equal_area $grib2_sample $tempGrib
${tools_dir}/grib_get -p projString $tempGrib > $tempText
grep -q "proj=laea" $tempText

# (conic projections with standard parallels, not the equator: as PROJ)
${tools_dir}/grib_set -s gridType=lambert,Latin1InDegrees=45,Latin2InDegrees=45 $grib2_sample $tempGrib
${tools_dir}/grib_get -p projString $tempGrib > $tempText
grep -q "proj=lcc" $tempText

${tools_dir}/grib_set -s gridType=polar_stereographic $grib2_sample $tempGrib
${tools_dir}/grib_get -p projString $tempGrib > $tempText
grep -q "proj=stere" $tempText

# Test invalid decode
set +e
${tools_dir}/grib_get -p projString:i $grib2_sample > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "ERROR.*Cannot unpack.*projTargetString.* as long" $tempText
grep -q "Hint: Try unpacking as string" $tempText

set +e
${tools_dir}/grib_get -p projString:d $grib2_sample > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "ERROR.*Cannot unpack.*projTargetString.* as double" $tempText
grep -q "Hint: Try unpacking as string" $tempText

# Albers
${tools_dir}/grib_set -s gridType=albers,Latin1InDegrees=30,Latin2InDegrees=60 $grib2_sample $tempGrib
${tools_dir}/grib_get -p projString $tempGrib

# Unsupported grids
unsupported_grids="space_view equatorial_azimuthal_equidistant transverse_mercator"
for ug in $unsupported_grids; do
  ${tools_dir}/grib_set -s gridType=$ug $grib2_sample $tempGrib
  set +e
  ${tools_dir}/grib_get -p projString $tempGrib > $tempText 2>&1
  status=$?
  set -e
  [ $status -ne 0 ]
  # (eccodes, or eckit::geo)
  grep -q -e "not yet implemented" -e "projString (Problem with calculation of geographic attributes)" $tempText
done

# Clean up
rm -f $tempGrib $tempText
