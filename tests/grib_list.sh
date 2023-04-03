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

label="grib_list_test"
tempFilt=temp.$label.filt
tempText=temp.$label.txt
tempGrib=temp.$label.grib
tempRef=temp.$label.ref

cat > $tempFilt <<EOF
 set productDefinitionTemplateNumber=31;
 set numberOfContributingSpectralBands=4;
 set satelliteSeries={61,61,61,62};
 set satelliteNumber={4,4,4,5};
 set instrumentType={591,591,591,592};
 set scaleFactorOfCentralWaveNumber={3,3,3,4};
 set scaledValueOfCentralWaveNumber={26870,9272,8377,22334};
 write "$tempGrib";
EOF

file="${data_dir}/reduced_gaussian_model_level.grib2"

${tools_dir}/grib_filter  $tempFilt $file

cat >$tempFilt<<EOF
 print("$tempText") "productDefinitionTemplateNumber=[productDefinitionTemplateNumber]";
 print("$tempText") "numberOfContributingSpectralBands=[numberOfContributingSpectralBands]";
 print("$tempText") "satelliteSeries={[satelliteSeries',']}";
 print("$tempText") "satelliteNumber={[satelliteNumber',']}";
 print("$tempText") "instrumentType={[instrumentType',']}";
 print("$tempText") "scaleFactorOfCentralWaveNumber={[scaleFactorOfCentralWaveNumber',']}";
 print("$tempText") "scaledValueOfCentralWaveNumber={[scaledValueOfCentralWaveNumber',']}";
EOF

${tools_dir}/grib_filter $tempFilt $tempGrib

cat >$tempRef<<EOF
productDefinitionTemplateNumber=31
numberOfContributingSpectralBands=4
satelliteSeries={61,61,61,62}
satelliteNumber={4,4,4,5}
instrumentType={591,591,591,592}
scaleFactorOfCentralWaveNumber={3,3,3,4}
scaledValueOfCentralWaveNumber={26870,9272,8377,22334}
EOF

diff $tempRef $tempText

rm -f $tempText $tempRef $tempFilt $tempGrib $tempText
