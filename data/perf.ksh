#!/bin/ksh

file=collection.grib1
#file=exp/performance/16bpv.grib
file=x.grib

set -A versions 1.2.0 1.3.0 1.4.0 1.5.0 1.6.0

for version in ${versions[@]}
do
  echo =======================
  echo time /usr/local/lib/metaps/lib/grib_api/${version}/bin/grib_dump -O $file
  time /usr/local/lib/metaps/lib/grib_api/${version}/bin/grib_dump -O $file 2> /dev/null > /dev/null
  echo
done


