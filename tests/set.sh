#!/bin/sh
# Copyright 2005-2013 ECMWF.
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

rm -f $outfile || true

${tools_dir}grib_set -v -p levtype,centre,levtype:l,centre:l -s levtype=pl,centre=80 $infile $outfile >$REDIRECT

levtype=`${tools_dir}grib_get -p levtype $outfile`
[ $levtype = "pl" ]

centre=`${tools_dir}grib_get -p centre $outfile`
[ $centre = "cnmc" ]

levtype=`${tools_dir}grib_get -p levtype:l $outfile`
[ $levtype -eq 100 ]

centre=`${tools_dir}grib_get -p centre:l $outfile`
[ $centre -eq 80 ]

rm -f $outfile || true

# Create the grib2 file from grib1
grib1File=${data_dir}/regular_gaussian_surface.grib1
grib2File=${data_dir}/regular_gaussian_surface.grib2
${tools_dir}grib_set -s editionNumber=2 $grib1File $grib2File

infile=$grib2File
outfile=${data_dir}/set.grib2

rm -f $outfile || true

${tools_dir}grib_set -v -p levtype:l,centre:s  -s levtype:l=100,centre:s=cnmc $infile $outfile >$REDIRECT

levtype=`${tools_dir}grib_get -p levtype $outfile`
[ $levtype = "pl" ]

centre=`${tools_dir}grib_get -p centre $outfile`
[ $centre = "cnmc" ]

levtype=`${tools_dir}grib_get -p levtype:l $outfile`
[ $levtype -eq 100 ]

centre=`${tools_dir}grib_get -p centre:l $outfile`
[ $centre -eq 80 ]

set +e

${tools_dir}grib_set -p levtype $infile $outfile 2> $REDIRECT > $REDIRECT

[ $? -ne 0 ]

rm -f $outfile $grib2File || true
