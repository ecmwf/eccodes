#!/bin/sh
# Copyright 2005-2018 ECMWF.
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
outfile=${data_dir}/set.grib1

rm -f $outfile

${tools_dir}/grib_set -v -p levtype,centre,levtype:l,centre:l -s levtype=pl,centre=80 $infile $outfile >$REDIRECT

levtype=`${tools_dir}/grib_get -p levtype $outfile`
[ $levtype = "pl" ]

centre=`${tools_dir}/grib_get -p centre $outfile`
[ $centre = "cnmc" ]

levtype=`${tools_dir}/grib_get -p levtype:l $outfile`
[ $levtype -eq 100 ]

centre=`${tools_dir}/grib_get -p centre:l $outfile`
[ $centre -eq 80 ]

rm -f $outfile

infile=${data_dir}/regular_gaussian_surface.grib2
outfile=${data_dir}/set.grib2

rm -f $outfile

${tools_dir}/grib_set -v -p levtype:l,centre:s  -s levtype:l=100,centre:s=cnmc $infile $outfile >$REDIRECT

levtype=`${tools_dir}/grib_get -p levtype $outfile`
[ $levtype = "pl" ]

centre=`${tools_dir}/grib_get -p centre $outfile`
[ $centre = "cnmc" ]

levtype=`${tools_dir}/grib_get -p levtype:l $outfile`
[ $levtype -eq 100 ]

centre=`${tools_dir}/grib_get -p centre:l $outfile`
[ $centre -eq 80 ]

set +e
# This is expected to fail
${tools_dir}/grib_set -p levtype $infile $outfile 2> $REDIRECT > $REDIRECT
[ $? -ne 0 ]
set -e

# GRIB-941: encoding of grib2 angles
angleInDegrees=130.9989
angleInMicroDegrees=130998900
files="GRIB2.tmpl regular_gg_pl_grib2.tmpl reduced_gg_pl_320_grib2.tmpl polar_stereographic_pl_grib2.tmpl"
for f in $files; do
    f=$ECCODES_SAMPLES_PATH/$f
    ${tools_dir}/grib_set -s longitudeOfFirstGridPointInDegrees=$angleInDegrees $f $outfile
    grib_check_key_equals $outfile longitudeOfFirstGridPoint $angleInMicroDegrees
done

# GRIB-943: centre code table
${tools_dir}/grib_set -s centre=289 $ECCODES_SAMPLES_PATH/GRIB2.tmpl $outfile
${tools_dir}/grib_dump -O $outfile | grep -q 'centre = 289.*Zambia'


rm -f $outfile
