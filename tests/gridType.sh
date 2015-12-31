#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

REDIRECT=/dev/null

tmpdata=grib_api.gridType.grib
rm -f $tmpdata

${tools_dir}grib_set -s gridType=regular_gg ${data_dir}/reduced_latlon_surface.grib1 ${tmpdata} > $REDIRECT

gridType=`${tools_dir}grib_get -p gridType $tmpdata`
if [ $gridType != "regular_gg" ]
then
    echo "Unable to change from reduced_latlon to regular_gg"
    echo $gridType
    exit 1
fi

rm -f $tmpdata

${tools_dir}grib_set -s gridType=reduced_gg ${data_dir}/regular_gaussian_pressure_level.grib1 ${tmpdata} > $REDIRECT

gridType=`${tools_dir}grib_get -p gridType $tmpdata`
if [ $gridType != "reduced_gg" ]
then
    echo "Unable to change from regular_gg to reduced_gg"
    echo $gridType
    exit 1
fi

rm -f $tmpdata

###########
# gridName
###########
for f in $ECCODES_SAMPLES_PATH/regular_gg_ml_grib*tmpl; do
    gname=`${tools_dir}grib_get -p gridName $f`
    [ "$gname" = "F32" ]
done
for f in $ECCODES_SAMPLES_PATH/reduced_gg_pl_512_grib*.tmpl; do
    gname=`${tools_dir}grib_get -p gridName $f`
    [ "$gname" = "N512" ]
done

