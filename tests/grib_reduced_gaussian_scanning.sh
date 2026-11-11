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

# ---------------------------------------------------------
# Reduced Gaussian grids scanned south to north (jScansPositively=1)
# or east to west (iScansNegatively=1). See GitHub issue #654.
# Both must give the same points as scanning mode 0, in their own order.
# ---------------------------------------------------------

if [ $HAVE_GEOGRAPHY -eq 0 ]; then
    exit 0
fi

label="grib_reduced_gaussian_scanning_test"
tempGrib=temp.$label.grib
tempRef=temp.$label.ref
tempOut=temp.$label.out
tempSorted1=temp.$label.sorted1
tempSorted2=temp.$label.sorted2

sample=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl

get_points()
{
    ${tools_dir}/grib_get_data -L "%.6f %.6f" $1 | grep -v Lat | awk '{print $1, $2}'
}

same_points()
{
    sort $1 > $tempSorted1
    sort $2 > $tempSorted2
    cmp $tempSorted1 $tempSorted2
}

# Scanning mode 0: north to south, west to east
get_points $sample > $tempRef
numberOfPoints=$(${tools_dir}/grib_get -p numberOfPoints $sample)
[ $(wc -l < $tempRef) -eq $numberOfPoints ]

# South to north
${tools_dir}/grib_set -s jScansPositively=1,latitudeOfFirstGridPoint=-87863799,latitudeOfLastGridPoint=87863799 \
    $sample $tempGrib
[ $(${tools_dir}/grib_get -p global $tempGrib) -eq 1 ]
get_points $tempGrib > $tempOut
same_points $tempRef $tempOut
# The first point is on the southernmost latitude and latitudes never decrease
head -1 $tempOut | awk '{ if ($1 != "-87.863799") exit 1 }'
awk 'NR > 1 && $1 < prev { exit 1 } { prev = $1 }' $tempOut
${tools_dir}/grib_get_data $tempGrib > /dev/null

# East to west
${tools_dir}/grib_set -s iScansNegatively=1,longitudeOfFirstGridPoint=357187500,longitudeOfLastGridPoint=0 \
    $sample $tempGrib
[ $(${tools_dir}/grib_get -p global $tempGrib) -eq 1 ]
get_points $tempGrib > $tempOut
same_points $tempRef $tempOut
# The first point is on the northernmost latitude, and longitudes decrease along each row
head -1 $tempOut | awk '{ if ($1 != "87.863799") exit 1 }'
awk 'NR > 1 && $1 == prevlat && $2 >= prevlon { exit 1 } { prevlat = $1; prevlon = $2 }' $tempOut

# Both
${tools_dir}/grib_set -s jScansPositively=1,iScansNegatively=1,latitudeOfFirstGridPoint=-87863799,latitudeOfLastGridPoint=87863799,longitudeOfFirstGridPoint=357187500,longitudeOfLastGridPoint=0 \
    $sample $tempGrib
[ $(${tools_dir}/grib_get -p global $tempGrib) -eq 1 ]
get_points $tempGrib > $tempOut
same_points $tempRef $tempOut
head -1 $tempOut | awk '{ if ($1 != "-87.863799") exit 1 }'
awk 'NR > 1 && $1 < prev { exit 1 } { prev = $1 }' $tempOut
awk 'NR > 1 && $1 == prevlat && $2 >= prevlon { exit 1 } { prevlat = $1; prevlon = $2 }' $tempOut

# Clean up
rm -f $tempGrib $tempRef $tempOut $tempSorted1 $tempSorted2
