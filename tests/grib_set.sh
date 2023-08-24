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

REDIRECT=/dev/null

infile=${data_dir}/regular_gaussian_surface.grib1
outfile=${data_dir}/temp.grib_set.grib
temp=temp.grib_set.out

rm -f $outfile

${tools_dir}/grib_set -v -p levtype,centre,levtype,centre:l -s levtype=pl,centre=80 $infile $outfile >$REDIRECT

levtype=`${tools_dir}/grib_get -p levtype $outfile`
[ $levtype = "pl" ]

centre=`${tools_dir}/grib_get -p centre $outfile`
[ $centre = "cnmc" ]

centre=`${tools_dir}/grib_get -p centre:l $outfile`
[ $centre -eq 80 ]

rm -f $outfile

infile=${data_dir}/regular_gaussian_surface.grib2
outfile=${data_dir}/set.grib2

rm -f $outfile

${tools_dir}/grib_set -v -p levtype:s,centre:s  -s typeOfLevel=isobaricInhPa,centre:s=cnmc $infile $outfile >$REDIRECT

levtype=`${tools_dir}/grib_get -p levtype $outfile`
[ $levtype = "pl" ]

centre=`${tools_dir}/grib_get -p centre $outfile`
[ $centre = "cnmc" ]

centre=`${tools_dir}/grib_get -p centre:l $outfile`
[ $centre -eq 80 ]

# Set without -s. Expected to fail
# ----------------------------------------------------
set +e
${tools_dir}/grib_set -p levtype $infile $outfile > $temp 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "provide some keys to set" $temp

# Set with empty -s. Expected to fail
# ----------------------------------------------------
set +e
${tools_dir}/grib_set -s '' $infile $outfile > $temp 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "provide some keys to set" $temp

# Out-of-bounds value. Expected to fail
# ----------------------------------------------------
input=${data_dir}/reduced_gaussian_sub_area.grib2
set +e
${tools_dir}/grib_set -s perturbationNumber=1000 $input $outfile 2>$temp
status=$?
set -e
[ $status -ne 0 ]
grep -q "Trying to encode value of 1000 but the maximum allowable value is 255 (number of bits=8)" $temp

# Negative value for an unsigned key. Expected to fail
# ----------------------------------------------------
input=${data_dir}/reduced_gaussian_sub_area.grib2
set +e
${tools_dir}/grib_set -s perturbationNumber=-1 $input $outfile 2>$temp
status=$?
set -e
[ $status -ne 0 ]
grep -q "Trying to encode a negative value of -1 for key of type unsigned" $temp

# ECC-1605: Out-of-bounds value for signed keys
# ----------------------------------------------------
if [ $ECCODES_ON_WINDOWS -eq 0 ]; then
    input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
    set +e
    ${tools_dir}/grib_set -s forecastTime=2147483648 $input $outfile 2>$temp
    status=$?
    set -e
    [ $status -ne 0 ]
    grep -q "Trying to encode value of 2147483648 but the allowable range is -2147483647 to 2147483647" $temp

    set +e
    ${tools_dir}/grib_set -s forecastTime=-2147483650 $input $outfile 2>$temp
    status=$?
    set -e
    [ $status -ne 0 ]
fi

# GRIB-941: encoding of GRIB2 angles
# -----------------------------------
angleInDegrees=130.9989
angleInMicroDegrees=130998900
files="GRIB2.tmpl regular_gg_pl_grib2.tmpl reduced_gg_pl_320_grib2.tmpl polar_stereographic_pl_grib2.tmpl"
for f in $files; do
    f=$ECCODES_SAMPLES_PATH/$f
    ${tools_dir}/grib_set -s longitudeOfFirstGridPointInDegrees=$angleInDegrees $f $outfile
    grib_check_key_equals $outfile longitudeOfFirstGridPoint $angleInMicroDegrees
done

# GRIB-943: centre code table
# ----------------------------
${tools_dir}/grib_set -s centre=289 $ECCODES_SAMPLES_PATH/GRIB2.tmpl $outfile
${tools_dir}/grib_dump -O $outfile > $temp
grep -q 'centre = 289.*Zambia' $temp

# ECC-539: avoid output being the same as input
# -----------------------------------------------
set +e
${tools_dir}/grib_set -s centre=0 $outfile $outfile
status=$?
set -e
[ $status -ne 0 ]

# offsetValuesBy
# ------------------
input=${data_dir}/reduced_latlon_surface.grib2
${tools_dir}/grib_set -s offsetValuesBy=0.5  $input $outfile

max=`${tools_dir}/grib_get -F%.3f -p max $input`
[ "$max" = "12.597" ]
max=`${tools_dir}/grib_get -F%.3f -p max $outfile`
[ "$max" = "13.097" ]

# ECC-1359: string that can be converted to an integer
# ---------------------------------------------------
${tools_dir}/grib_set -s month:s=6 $ECCODES_SAMPLES_PATH/GRIB2.tmpl $outfile
grib_check_key_equals $outfile month 6
# Now try an illegal value: a string that cannot be converted to an integer
set +e
${tools_dir}/grib_set -s month=BAD $ECCODES_SAMPLES_PATH/GRIB2.tmpl $outfile 2> $temp
status=$?
set -e
[ $status -ne 0 ]
grep -q "String cannot be converted to an integer" $temp


# ECC-1363: Does not fail for invalid value for key of type 'double'
# ------------------------------------------------------------------
${tools_dir}/grib_set -s angleOfRotation:s=10.66 $ECCODES_SAMPLES_PATH/rotated_ll_sfc_grib2.tmpl $outfile
grib_check_key_equals $outfile angleOfRotation 10.66
# Now try an illegal value: a string that cannot be converted to an integer
set +e
${tools_dir}/grib_set -s angleOfRotation=BAD $ECCODES_SAMPLES_PATH/rotated_ll_sfc_grib2.tmpl $outfile 2>$temp
status=$?
set -e
[ $status -ne 0 ]
grep -q "String cannot be converted to a double" $temp


# Set ascii key via double or long
# --------------------------------
${tools_dir}/grib_set -s setLocalDefinition=1,localDefinitionNumber=21 $ECCODES_SAMPLES_PATH/GRIB2.tmpl $outfile
${tools_dir}/grib_set -s marsDomain=x $outfile $temp
grib_check_key_equals $temp 'marsDomain' 'x'
set +e
${tools_dir}/grib_set -s marsDomain=9 $outfile $temp
status=$?
set -e
[ $status -ne 0 ]

set +e
${tools_dir}/grib_set -s marsDomain=1.2 $outfile $temp
status=$?
set -e
[ $status -ne 0 ]

# Strict option
# ---------------
# There is only one field in this file with shortName=2t
input=${data_dir}/tigge_cf_ecmwf.grib2
# This copies all messages to the output changing one of them
${tools_dir}/grib_set -w shortName=2t -s offsetValuesBy=0.5  $input $outfile
count=`${tools_dir}/grib_count $outfile`
[ $count -eq 43 ]
# Now we copy only what was changed
${tools_dir}/grib_set -w shortName=2t -S -s offsetValuesBy=0.5  $input $outfile
count=`${tools_dir}/grib_count $outfile`
[ $count -eq 1 ]
grib_check_key_equals $outfile shortName '2t'


# Clean up
rm -f $outfile $temp
