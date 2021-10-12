#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

REDIRECT=/dev/null

infile=${data_dir}/regular_gaussian_surface.grib1
outfile=${data_dir}/temp.grib_set.grib
temp=temp.grib_set.out

rm -f $outfile

${tools_dir}/grib_set -v -p levtype,centre,levtype:l,centre:l -s levtype=pl,centre=80 $infile $outfile >$REDIRECT

levtype=`${tools_dir}/grib_get -p levtype $outfile`
[ $levtype = "pl" ]

centre=`${tools_dir}/grib_get -p centre $outfile`
[ $centre = "cnmc" ]

#levtype=`${tools_dir}/grib_get -p levtype:l $outfile`
#[ $levtype -eq 100 ]

centre=`${tools_dir}/grib_get -p centre:l $outfile`
[ $centre -eq 80 ]

rm -f $outfile

infile=${data_dir}/regular_gaussian_surface.grib2
outfile=${data_dir}/set.grib2

rm -f $outfile

${tools_dir}/grib_set -v -p levtype:l,centre:s  -s levtype=pl,centre:s=cnmc $infile $outfile >$REDIRECT

levtype=`${tools_dir}/grib_get -p levtype $outfile`
[ $levtype = "pl" ]

centre=`${tools_dir}/grib_get -p centre $outfile`
[ $centre = "cnmc" ]

#levtype=`${tools_dir}/grib_get -p levtype:l $outfile`
#[ $levtype -eq 100 ]

centre=`${tools_dir}/grib_get -p centre:l $outfile`
[ $centre -eq 80 ]

# Set without -s. Expected to fail
# ----------------------------------------------------
set +e
${tools_dir}/grib_set -p levtype $infile $outfile 2> $REDIRECT > $REDIRECT
status=$?
set -e
[ $status -ne 0 ]

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
${tools_dir}/grib_dump -O $outfile | grep -q 'centre = 289.*Zambia'

# ECC-539: avoid output being the same as input
# -----------------------------------------------
set +e
${tools_dir}/grib_set -s centre=0 $outfile $outfile
status=$?
set -e
[ $status -ne 0 ]


rm -f $outfile $temp
