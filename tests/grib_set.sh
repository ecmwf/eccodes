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

label="grib_set_test"
REDIRECT=/dev/null

infile=${data_dir}/regular_gaussian_surface.grib1
outfile=${data_dir}/temp.$label.grib
temp=${data_dir}/temp.$label.out

rm -f $outfile

${tools_dir}/grib_set -V

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

# offsetValuesBy
# ------------------
input=${data_dir}/reduced_latlon_surface.grib2
${tools_dir}/grib_set -s offsetValuesBy=0.5  $input $outfile

max=`${tools_dir}/grib_get -F%.3f -p max $input`
[ "$max" = "12.597" ]
max=`${tools_dir}/grib_get -F%.3f -p max $outfile`
[ "$max" = "13.097" ]


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

# Key with no_fail flag
# ------------------------
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
grib_check_key_equals $input 'typeOfProcessedData:i' '2'
${tools_dir}/grib_set -s typeOfProcessedData=rubbish $input $outfile
grib_check_key_equals $outfile 'typeOfProcessedData:i' '255' # set to default

# GDSPresent
# ------------
input=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
${tools_dir}/grib_set -s GDSPresent=1 $input $outfile


# Clean up
rm -f $outfile $temp
