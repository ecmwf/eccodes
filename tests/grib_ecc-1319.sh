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
set -u

label="grib_ecc-1319-test"
temp=temp.$label.grib

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

lev_types="
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


for lt in $lev_types; do
    ${tools_dir}/grib_set -s tablesVersion=27,typeOfLevel=$lt $sample_grib2 $temp
    ltype=`${tools_dir}/grib_get -p mars.levtype $temp`
    check_levtype $ltype
done


rm -f $temp
