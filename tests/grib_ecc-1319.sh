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

label="grib_ecc-1319_test"

temp=temp.$label.grib
temp1=temp1.$label.grib
temp2=temp2.$label.grib
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

typeOfLevels="
surface
tropopause
nominalTop
mostUnstableParcel
mixedLayerParcel
isobaricInPa
isobaricInhPa
isobaricLayer
meanSea
heightAboveGround
heightAboveGroundLayer
hybrid
hybridHeight
hybridLayer
depthBelowLand
depthBelowLandLayer
theta
thetaLayer
potentialVorticity
soil
soilLayer
entireAtmosphere
entireOcean
snow
snowLayer
seaIce
seaIceLayer"

ok_levtypes="hhl ml o2d pl pt pv sfc sol"
check_levtype()
{
    _arg=$1
    result=1
    for n in $ok_levtypes; do
        if [ "$_arg" = "$n" ]; then result=0; break; fi
    done
    return $result
}


for a_typeOfLevel in $typeOfLevels; do
    ${tools_dir}/grib_set -s tablesVersion=27,typeOfLevel=$a_typeOfLevel $sample_grib2 $temp
    levtype=`${tools_dir}/grib_get -p mars.levtype $temp`
    check_levtype $levtype
done

# Also check specific cases
# --------------------------
# 260649 = Sea ice surface temperature (UERRA)
${tools_dir}/grib_set -s productionStatusOfProcessedData=8 $sample_grib2 $temp1
${tools_dir}/grib_set -s paramId=260649 $temp1 $temp2
grib_check_key_equals $temp2 levtype sfc
# 260650 = Snow on ice total depth (UERRA)
${tools_dir}/grib_set -s paramId=260650 $temp1 $temp2
grib_check_key_equals $temp2 levtype sfc

# Lake ice surface temperature
${tools_dir}/grib_set -s paramId=228013 $sample_grib2 $temp
grib_check_key_equals $temp levtype sfc
# Lake ice total depth
${tools_dir}/grib_set -s paramId=228014 $sample_grib2 $temp
grib_check_key_equals $temp levtype sfc



rm -f $temp $temp1 $temp2
