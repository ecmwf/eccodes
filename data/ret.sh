#!/bin/ksh

set -eu

set -A files            \
  reduced_latlon_surface.grib1 \
  reduced_gaussian_pressure_level.grib1 \
  regular_gaussian_pressure_level.grib1 \
  reduced_gaussian_model_level.grib1  \
  regular_gaussian_model_level.grib1  \
  reduced_gaussian_surface.grib1 \
  regular_gaussian_surface.grib1 \
  regular_latlon_surface.grib1 \
  spherical_pressure_level.grib1 \
  spherical_model_level.grib1 

set -A rets   \
  "ret,stream=wave,levtype=sfc,param=swh," \
  "ret,stream=oper,level=1000,gaussian=reduced,grid=32,param=t," \
  "ret,level=1000,gaussian=regular,grid=32,param=t," \
  "ret,levtype=ml,level=1,gaussian=reduced,grid=32,param=t," \
  "ret,levtype=ml,level=1,gaussian=regular,grid=32,param=t," \
  "ret,levtype=sfc,param=2t,gaussian=reduced,grid=32," \
  "ret,levtype=sfc,param=2t,gaussian=regular,grid=32," \
  "ret,levtype=sfc,param=2t,grid=2/2,area=60/0/0/30,"  \
  "ret,class=od,type=an,stream=da,expver=0001,levtype=pl,levelist=1000,param=130,time=1200,step=00,domain=g,resol=63,param=t," \
  "ret,class=od,type=an,stream=da,expver=0001,levtype=ml,levelist=1,param=130,time=1200,step=00,domain=g,resol=63,param=t," 

set -A precision  2 2 2 2 2 2 2 -1 -1 -1 

[[ ${#files[@]} -eq ${#rets[@]} ]] && [[ ${#rets[@]} -eq ${#precision[@]} ]]

for (( i=0; i< ${#files[@]}; i++ ))    
do

  mars << EOF
  ${rets[i]}
  tar="full_${files[i]}"
EOF

  if [[ ${precision[i]} -gt 0 ]] 
  then 
    grib_set -r -s bitsPerValue=0,decimalScaleFactor=${precision[i]} full_${files[i]} ${files[i]}
    rm -f full_${files[i]}
  else
    mv full_${files[i]} ${files[i]}
  fi

  grib_dump ${files[i]} > /dev/null
  grib2=`basename ${files[i]} .grib1`.grib2
  grib_set -s editionNumber=2 ${files[i]} $grib2
  grib_dump ${grib2} > /dev/null

done
