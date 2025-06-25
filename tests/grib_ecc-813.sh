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
# This is the test for JIRA issue ECC-813
# grib_compare reports differences twice (when two timeRange blocks exist)
# ---------------------------------------------------------

REDIRECT=/dev/null

label="grib_ecc-813_test"
tempGribA=temp.$label.A.grib
tempGribB=temp.$label.B.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref

sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}/grib_set -s stepType=accum,numberOfTimeRanges=2 $sample $tempGribA
${tools_dir}/grib_set -s stepType=accum,numberOfTimeRanges=1 $sample $tempGribB

set +e
${tools_dir}/grib_compare $tempGribA $tempGribB > $tempOut
status=$?
set -e
[ $status -ne 0 ]

cat > $tempRef << EOF

-- GRIB #1 -- shortName=t paramId=130 stepRange=0 levelType=sfc level=0 packingType=grid_simple gridType=regular_ll --
long [totalLength]: [215] != [203]
long [section4Length]: [70] != [58]
long [numberOfTimeRanges]: [2] != [1]
Different size for "typeOfStatisticalProcessing"  [2]  [1]
Different size for "typeOfTimeIncrement"  [2]  [1]
Different size for "indicatorOfUnitForTimeRange"  [2]  [1]
Different size for "lengthOfTimeRange"  [2]  [1]
Different size for "indicatorOfUnitForTimeIncrement"  [2]  [1]
Different size for "timeIncrement"  [2]  [1]
EOF
diff $tempRef $tempOut


# Clean up
rm -f $tempGribA $tempGribB $tempFilt $tempLog $tempOut $tempRef
