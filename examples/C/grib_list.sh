#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

label="examples_c_grib_list"
tempRef=temp.$label.ref
tempText=temp.$label.txt

cat > $tempRef <<EOF
numberOfContributingSpectralBands=9
count=9
scaledValueOfCentralWaveNumber 0 = 26870
scaledValueOfCentralWaveNumber 1 = 9272
scaledValueOfCentralWaveNumber 2 = MISSING
scaledValueOfCentralWaveNumber 3 = MISSING
scaledValueOfCentralWaveNumber 4 = MISSING
scaledValueOfCentralWaveNumber 5 = MISSING
scaledValueOfCentralWaveNumber 6 = MISSING
scaledValueOfCentralWaveNumber 7 = MISSING
scaledValueOfCentralWaveNumber 8 = MISSING
Now scaledValueOfCentralWaveNumber 0 = 1000
Now scaledValueOfCentralWaveNumber 1 = 1001
Now scaledValueOfCentralWaveNumber 2 = 1002
Now scaledValueOfCentralWaveNumber 3 = 1003
Now scaledValueOfCentralWaveNumber 4 = 1004
Now scaledValueOfCentralWaveNumber 5 = 1005
Now scaledValueOfCentralWaveNumber 6 = 1006
Now scaledValueOfCentralWaveNumber 7 = 1007
Now scaledValueOfCentralWaveNumber 8 = 1008
EOF

${examples_dir}/c_grib_list > $tempText
diff $tempRef $tempText

# Clean up
rm -f $tempText $tempRef
