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

infile=${data_dir}/regular_latlon_surface.grib1
shdata=${data_dir}/spherical_model_level.grib1
suff=_ieee_test.grib1
outsimple=simple$suff
out32=32$suff
out64=64$suff

cat > w.filter <<EOF
  set Nj = 2;
  set latitudeOfFirstGridPointInDegrees = 60;
  set latitudeOfLastGridPointInDegrees = 59;
  set Ni = 3;
  set longitudeOfFirstGridPointInDegrees = 0;
 set longitudeOfLastGridPointInDegrees = 2;
  set DjInDegrees = 1;
  set DiInDegrees = 1;
  set values={1.234567e-36,2.34567e-36,1.234567e-36,2.34567e-36,1.234567e-36,2.34567e-36};
  write;
EOF

cat > r.filter <<EOF
print "[values%g]";
EOF

${tools_dir}grib_filter -o $outsimple w.filter $infile 

${tools_dir}grib_filter r.filter $outsimple > $outsimple.txt
diff $outsimple.txt ${data_dir}/ieee_test.good

${tools_dir}grib_set -r -s packingType=grid_ieee $outsimple $out32
${tools_dir}grib_filter r.filter $out32 > $out32.txt 
diff $out32.txt ${data_dir}/ieee_test.good

# Disabled for now. See GRIB-2 and GRIB-80
#GRIB_IEEE_PACKING=32
#export GRIB_IEEE_PACKING
#${tools_dir}grib_filter -o $out32 w.filter $infile 
#${tools_dir}grib_filter r.filter $out32 > $out32.txt 
#diff $out32.txt ${data_dir}/ieee_test.good

#GRIB_IEEE_PACKING=64
#export GRIB_IEEE_PACKING
#${tools_dir}grib_filter -o $out64 w.filter $infile 
#${tools_dir}grib_filter r.filter $out64 > $out64.txt 
#diff $out64.txt ${data_dir}/ieee_test.good

rm -f $outsimple $out32 $out64 $out32.txt $out64.txt
rm -f ${data_dir}/$outsimple.txt ${data_dir}/$out32.txt ${data_dir}/$out64.txt 
rm -f w.filter $outsimple.txt

${tools_dir}grib_filter r.filter $shdata > $shdata.txt
diff $shdata.txt $shdata.good

${tools_dir}grib_set -r -s packingType=grid_ieee $shdata ${shdata}_ieee
${tools_dir}grib_filter r.filter ${shdata}_ieee > $shdata.txt
diff $shdata.txt $shdata.good
rm -f ${shdata}_ieee

##########################
#GRIB_IEEE_PACKING=32
#export GRIB_IEEE_PACKING
#${tools_dir}grib_copy -r $shdata ${shdata}_32
#${tools_dir}grib_filter r.filter ${shdata}_32 > ${shdata}_32.txt
#diff ${shdata}_32.txt ${shdata}_32.good

#GRIB_IEEE_PACKING=64
#export GRIB_IEEE_PACKING
#${tools_dir}grib_copy -r $shdata ${shdata}_64
#${tools_dir}grib_filter r.filter ${shdata}_64 > ${shdata}_64.txt
#diff ${shdata}_64.txt ${shdata}.good

rm -f r.filter ${shdata}_64.txt ${shdata}_32.txt $shdata.txt
